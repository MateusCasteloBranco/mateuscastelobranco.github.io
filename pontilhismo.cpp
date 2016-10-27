#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ctime>
#include <cstdlib>

using namespace std;
using namespace cv;

#define STEP1 4
#define JITTER1 3
#define RAIO1 4

#define STEP2 2
#define JITTER2 2
#define RAIO2 1

Mat image, frame, points, points2, border;

Mat soma,saturado;

vector<Mat> planes1,planes2;


int main(int argc, char** argv)
{
  vector<int> yrange1;
  vector<int> xrange1;

  vector<int> yrange2;
  vector<int> xrange2;

  int width, height, gray2, r,g,b;
  int x, y;

  frame = imread("teste4.png",CV_LOAD_IMAGE_COLOR);

  namedWindow("image",WINDOW_NORMAL);
  imshow("image", frame);
  imwrite("image1.jpg", frame);
  split(frame, planes1);
  Canny(frame, border, 75, 100);

  srand(time(0));

  if(!frame.data)
  {
	cout << "nao abriu" << argv[1] << endl;
    cout << argv[0] << " imagem.jpg";
    exit(0);
  }

  width=frame.size().width;
  height=frame.size().height;

  points = Mat(height, width, CV_8UC3, Scalar(255,255,255));
  points2 =  Mat(height, width, CV_8UC3, Scalar(255,255,255));

  cout<< width <<","<< height<<endl;

  xrange1.resize(height/STEP1);
  yrange1.resize(width/STEP1);

  iota(xrange1.begin(), xrange1.end(), 0);
  iota(yrange1.begin(), yrange1.end(), 0);

  for(uint i=0; i<xrange1.size(); i++)
  {
    xrange1[i]= xrange1[i]*STEP1+STEP1/2;
  }

  for(uint i=0; i<yrange1.size(); i++)
  {
    yrange1[i]= yrange1[i]*STEP1+STEP1/2;
  }

  random_shuffle(xrange1.begin(), xrange1.end());
  for(auto i : xrange1)
  {
    random_shuffle(yrange1.begin(), yrange1.end());
    for(auto j : yrange1)
    {
      x = i+rand()%(2*JITTER1)-JITTER1+1;
      y = j+rand()%(2*JITTER1)-JITTER1+1;

      b = planes1[0].at<uchar>(x,y);
      g = planes1[1].at<uchar>(x,y);
      r = planes1[2].at<uchar>(x,y);

      circle(points, cv::Point(y,x), RAIO1, CV_RGB(r,g,b), -1, CV_AA);
    }
  }



  xrange2.resize(height/STEP2);
  yrange2.resize(width/STEP2);

  iota(xrange2.begin(), xrange2.end(), 0);
  iota(yrange2.begin(), yrange2.end(), 0);

  for(uint i=0; i<xrange2.size(); i++)
  {
    xrange2[i]= xrange2[i]*STEP2+STEP2/2;
  }

  for(uint i=0; i<yrange2.size(); i++)
  {
    yrange2[i]= yrange2[i]*STEP2+STEP2/2;
  }

  random_shuffle(xrange2.begin(), xrange2.end());

  for(auto i : xrange2)
  {
    random_shuffle(yrange2.begin(), yrange2.end());
    for(auto j : yrange2)
    {
      x = i+rand()%(2*JITTER2)-JITTER2+1;
      y = j+rand()%(2*JITTER2)-JITTER2+1;
      gray2 = border.at<uchar>(x,y);
      circle(points2, cv::Point(y,x), RAIO2, CV_RGB(gray2,gray2,gray2), -1, CV_AA);
    }
  }

  addWeighted(points,1.0,points2,0.3,0.0,soma);

  namedWindow("soma",WINDOW_NORMAL);
  imshow("soma",soma);
  imwrite("soma1.jpg",soma);
  cvtColor(soma,saturado,CV_RGB2HSV);

  split(saturado,planes2);

  planes2[1].convertTo(planes2[1],CV_8UC1,1,25);

  merge(planes2,saturado);

  cvtColor(saturado,saturado,CV_HSV2RGB);

  namedWindow("saturado",WINDOW_NORMAL);
  imshow("saturado",saturado);
  imwrite("saturado1.jpg",saturado);

  waitKey();
  return 0;
}

