#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <assert.h> 

const int KEY_SPACE = 32;
const int KEY_ESC = 27;

CvHaarClassifierCascade *cascade;
CvMemStorage            *storage;

void detect(IplImage *img);

int main(int argc, char** argv)
{
  std::cout << "Using OpenCV " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_SUBMINOR_VERSION << std::endl;
  
  CvCapture *capture;
  IplImage  *frame;
  int input_resize_percent = 100;
  
 

  cascade = (CvHaarClassifierCascade*) cvLoad("/home/pi/Desktop/project/1.vehicle_detection/cars.xml", 0, 0, 0);
  storage = cvCreateMemStorage(0);
  capture = cvCaptureFromAVI("/home/pi/Desktop/project/1.vehicle_detection/video1.avi");

  assert(cascade && storage && capture);

  //cvNamedWindow("video", 1);

  IplImage* frame1 = cvQueryFrame(capture);
  frame = cvCreateImage(cvSize((int)((frame1->width*input_resize_percent)/100) , (int)((frame1->height*input_resize_percent)/100)), frame1->depth, frame1->nChannels);

  int key = 0;
  do
  {
    frame1 = cvQueryFrame(capture);

    if(!frame1)
      break;

    cvResize(frame1, frame);

    detect(frame);

    key = cvWaitKey(33);

    if(key == KEY_SPACE)
      key = cvWaitKey(0);

    if(key == KEY_ESC)
      break;

  }while(1);

  cvDestroyAllWindows();
  cvReleaseImage(&frame);
  cvReleaseCapture(&capture);
  cvReleaseHaarClassifierCascade(&cascade);
  cvReleaseMemStorage(&storage);

  return 0;
}

void detect(IplImage *img)
{
  CvSize img_size = cvGetSize(img);
  CvSeq *object = cvHaarDetectObjects(
    img,
    cascade,
    storage,
    1.1, //1.1,//1.5, //-------------------SCALE FACTOR
    1, //2        //------------------MIN NEIGHBOURS
    0, //CV_HAAR_DO_CANNY_PRUNING
    cvSize(0,0),//cvSize( 30,30), // ------MINSIZE
    img_size //cvSize(70,70)//cvSize(640,480)  //---------MAXSIZE
    );

  std::ofstream ofs ("count.txt", std::ofstream::out);
  ofs << "Total: " << object->total << " cars detected." << std::endl;
  ofs.close();
  system("sudo cp count.txt /var/www/html/count.txt");
  //std::cout << "Total: " << object->total << " cars detected." << std::endl;

  for(int i = 0 ; i < ( object ? object->total : 0 ) ; i++)
  {
    CvRect *r = (CvRect*)cvGetSeqElem(object, i);
    cvRectangle(img,
      cvPoint(r->x, r->y),
      cvPoint(r->x + r->width, r->y + r->height),
      CV_RGB(255, 0, 0), 2, 8, 0);
  }

  //cvShowImage("video", img);
  cvSaveImage("output.png", img);
  system("sudo cp output.png /var/www/html/output.png");
}
