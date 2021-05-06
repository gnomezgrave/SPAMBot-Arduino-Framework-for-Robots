#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>

#define NR 60

using namespace std;
using namespace cv;
RNG rng(12345);
Ptr<CLAHE> p_clahe = createCLAHE(2.0, Size(8, 8));

typedef IplImage* (*callback_prototype)(IplImage*);

int thresh = 75, N = 11;

cv::Point2f center(0, 0);

cv::Mat templ;
double getPSNR(const Mat& I1, const Mat& I2) {
	Mat s1;
	absdiff(I1, I2, s1);       // |I1 - I2|
	s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
	s1 = s1.mul(s1);           // |I1 - I2|^2
	
	Scalar s = sum(s1);  // sum elements per channel
	
	double sse = s.val[0] + s.val[1] + s.val[2];  // sum channels
	
	if (sse <= 1e-10)  // for small values return zero
		return 0;
	else {
		double mse = sse / (double)(I1.channels() * I1.total());
		double psnr = 10.0 * log10((255 * 255) / mse);
		return psnr;
	}
}
Point MatchingMethod(Mat& img, int match_method) {
	Point pp(-1, -1);
	/// Source image to display
	
	Mat result;
	p_clahe->apply(img, img);
	/// Create the result matrix
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;
	result.create(result_cols, result_rows, CV_32FC1);
	
	/// Do the Matching and Normalize
	matchTemplate(img, templ, result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	
	/// Localizing the best match with minMaxLoc
	double minVal;
	double maxVal;
	Point minLoc;
	Point maxLoc;
	Point matchLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	
	/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all
	/// the other methods, the higher the better
	if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED) {
		matchLoc = minLoc;
	} else {
		matchLoc = maxLoc;
	}
	
	double psnr = getPSNR(
		img(cv::Rect(matchLoc.x, matchLoc.y, templ.cols, templ.rows)), templ);
	/// Show me what you got
	if (psnr > 8.0) {  // found a reasonable match
		// rectangle( img_out, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y
		// + templ.rows ), Scalar::all(255), 2, 8, 0 );
		
		pp = matchLoc;
	}
	
	return pp;
}
int getXAvg(vector<Point>& pp){
	int avg=0;
	for (int i=0;i<pp.size();i++)
	{
		avg+=pp[i].x;
	}
	avg/=pp.size();
	return avg;
}
int getFirstNotNullFromCenter(vector<Point>& pp)
{
	for (int i=pp.size()/2;i<pp.size();i++)
	{
		if (pp[i].x>=0){
			return i;
		}
	}
	return -1;
}
int getFirstNotNull(vector<Point>& pp)
{
	for (int i=0;i<pp.size();i++)
	{
		if (pp[i].x>=0){
			return i;
		}
	}
	return -1;
}
int getXAvgNotNullBelowCenter(vector<Point>& pp)
{
	int avg=0,count=0;
	for (int i=pp.size()/2;i<pp.size();i++)
	{
		if (pp[i].x >=0) {
			avg+=pp[i].x;
			count++;
		}
	}
	avg/=count;
	return avg;
}

void process_video(callback_prototype custom_cb) {
	// Initialize camera
	
	CvCapture* capture = 0;
	capture = cvCaptureFromCAM(1);
	if (!capture) {
		cerr<<"!!! Cannot open initialize webcam!\n";
		return;
	}
	
	// Create a window for the video
	cvNamedWindow("", CV_WINDOW_AUTOSIZE);
	
	IplImage* frame = 0;
	IplImage* gray_frame = 0;
	char key = 0;
	
	cv::Mat templ_gray;
	
	cv::cvtColor(templ, templ_gray, CV_BGR2GRAY, 1);
	templ = templ_gray;
	vector<Point> points;
	while (key != 27)  // ESC
	{
		frame = cvQueryFrame(capture);
		if (!frame) {
			cerr<<"!!! cvQueryFrame failed!\n";
			break;
		}
		
		cv::Mat img = cv::Mat(frame, false);
		cv::Mat img_out;
		cv::Mat img_gray;
		
		cv::cvtColor(img, img_gray, CV_BGR2GRAY, 1);
		int null_count = 0;
		points.clear();
		for (int i = 0; i < img.rows / NR; i++) {
			
			cv::Rect roi(0, NR * i, img.cols, NR);
			cv::Mat img_cropped = img_gray(roi);
			
			Point p = MatchingMethod(img_cropped, CV_TM_CCOEFF);
			if (p == Point(-1, -1)) {
				null_count++;
			}
			points.push_back(p);
			cout << p << " ";
		}
		cout << endl;
		cv::Rect roi;
		int x_avg=getXAvg(points);
		cout<<x_avg<<endl;
		if (x_avg>0) {
			if (null_count < 3){
				int i=getFirstNotNull(points);
				if (i>=0) {
					roi.x = x_avg;
                    roi.y = std::max(points[i].y +NR,0 );
					roi.height = templ.rows;
					roi.width = templ.cols;
				}
			} else {
				int i=getFirstNotNullFromCenter(points);
				if (i>=0) {
					roi.x = getXAvgNotNullBelowCenter(points);
                    roi.y = std::max(points[i].y +NR,0 );
					roi.height = templ.rows;
					roi.width = templ.cols;
				}
			}
			
            if (roi.area()>0) {
                Mat numb=img(roi);
                int y=templ.rows/2;
                int erosion_size=5;
                               Mat element = getStructuringElement(MORPH_ELLIPSE,
                                              Size( 2*erosion_size + 1,
                               2*erosion_size+1 ),
                                              Point( erosion_size, erosion_size )
                               );
                for (int i=1;i<6;i++){
                    int x=i*templ.cols/6;
                    Vec3b vec=numb.at<Vec3b>(Point(x,y));

                }
                imshow("", numb);
            }
		}
		/*    cv::cvtColor(img, img, CV_BGR2YUV);
		 *      std::vector<cv::Mat> channels;
		 *      cv::split(img, channels);
		 *      cv::equalizeHist(channels[0], channels[0]);
		 *      cv::merge(channels, img);
		 *      cv::cvtColor(img, img, CV_YUV2BGR);
		 * 
		 * 
		 *      cv::Mat img_edges=cv::Mat(img.rows, img.cols, CV_8UC1);
		 *      cv::Mat gray=cv::Mat(img.rows, img.cols, CV_8UC1);
		 *      cv::blur(img,img,cv::Size(3,3));
		 * 
		 * 
		 *      cv::cvtColor(img,gray,CV_BGR2GRAY,1);
		 *      vector< vector<Point> > squares;
		 * 
		 * 
		 *      int erosion_size=5;
		 *      Mat element = getStructuringElement(MORPH_ELLIPSE,
		 *				      Size( 2*erosion_size + 1,
		 *      2*erosion_size+1 ),
		 *				      Point( erosion_size, erosion_size )
		 *      );
		 * 
		 * 
		 *      /// Apply the dilation operation
		 *      erode( gray, gray, element );
		 * 
		 * 
		 *      // threshold( gray, gray,maxVal, 255,THRESH_BINARY);
		 * 
		 *      Canny(gray, img_edges, 50, 150, 3);*/
		/* Mat check_row=img.row(img.rows *0.66);
		 *      int counter=0;
		 *      for (int i=0;i<img.cols;i++) {
		 *      if (((unsigned char *)check_row.data)[i] == 1){
		 *      counter++;
	}
	((unsigned char *)img.data)[int(img.step*(img.rows *0.66)) +i*3] =0;
	}
	cout<<counter<<endl;
	*/
		/*vector<Vec4i> lines;
		 *      HoughLinesP(img_edges, lines, 1, CV_PI/180, 50, 40, 30 );
		 *      for( size_t i = 0; i < lines.size(); i++ )
		 *      {
		 *      Vec4i l = lines[i];
		 *      line( img, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3,
		 *      CV_AA);
	}
	*/
		// img = img <20;
		//    find_squares(img,squares);
		// bitwise_not(img,img);
		// find_squares(img,squares);
		
		// debugSquares(squares, img);
		
		key = cvWaitKey(10);
	}
	
	// Free memory
	cvDestroyWindow("result");
	cvReleaseCapture(&capture);
}

int main(int argc, char** argv) {
	// templ=imread(argv[1]);
	templ = imread("mask1.png");
	process_video(NULL);
	
	return 0;
}
