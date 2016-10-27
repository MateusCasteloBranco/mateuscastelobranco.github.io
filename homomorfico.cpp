#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cv.h>


using namespace cv;
using namespace std;



int gama_l = 1;
int gama_l_max = 100;
float gL;

int gama_h = 1;
int gama_h_max = 100;
float gH;

int c=0;
int c_max=500;
float C;

int D0=1;
int D0_max=200;
float d0;

char TrackbarName[50];

 int dft_A=640;
 int dft_B=480;

 //Mat tmp1 = Mat(dft_B, dft_A, CV_32F, Scalar::all(0));
 Mat tmp1;
 Mat padded, filter, mag;
 Mat imaginaryInput, complexImage, multsp;
 Mat image, imagegray, tmp;
 Mat_<float> realInput, zeros;
 vector<Mat> planos,planos2;



void on_trackbar_c(int, void*)
{
  while(D0==0){D0=D0+1;}
  C=c*0.01;
  float D;

  for(int i=0; i<tmp1.rows; i++)
  {
    for(int j=0; j<tmp1.cols; j++)
    {
       D=(j-tmp1.cols/2)*(j-tmp1.cols/2)+(i-tmp1.rows/2)*(i-tmp1.rows/2);
       float x=-1*C*(1.0*D/(D0*D0));
       tmp1.at<float> (i,j) = (gH-gL)*(1-exp(x)+gL);
    }
  }
  imshow("filtrada",tmp1);
  Mat comps[]= {tmp1, tmp1};
  merge(comps, 2, filter);
}
void on_trackbar_gama_l(int, void*)
{
  gL=gama_l*0.01;
  on_trackbar_c(c,0);
}
void on_trackbar_gama_h(int, void*)
{
  gH=gama_h*0.01;
  on_trackbar_c(c,0);
}
void on_trackbar_D0(int, void*)
{
  on_trackbar_c(c,0);
}

void deslocaDFT(Mat& image )
{
  Mat tmp, A, B, C, D;

  image = image(Rect(0, 0, image.cols & -2, image.rows & -2));
  int cx = image.cols/2;
  int cy = image.rows/2;

  A = image(Rect(0, 0, cx, cy));
  B = image(Rect(cx, 0, cx, cy));
  C = image(Rect(0, cy, cx, cy));
  D = image(Rect(cx, cy, cx, cy));

  A.copyTo(tmp);  D.copyTo(A);  tmp.copyTo(D);

  C.copyTo(tmp);  B.copyTo(C);  tmp.copyTo(B);
}

int main(int , char**){

  VideoCapture cap;

  int dft_M, dft_N;

  cap.open(0);
  if(!cap.isOpened())
    return -1;

  cap >> image;

  dft_M = getOptimalDFTSize(image.rows);
  dft_N = getOptimalDFTSize(image.cols);

  cout<<dft_M<<","<<dft_N<<endl;

  copyMakeBorder(image, padded, 0, dft_M - image.rows, 0, dft_N - image.cols, BORDER_CONSTANT, Scalar::all(0));

  zeros = Mat_<float>::zeros(padded.size());

  complexImage = Mat(padded.size(), CV_32FC2, Scalar(0));

  filter = complexImage.clone();

  tmp1 = Mat(dft_M, dft_N, CV_32F);

  Mat comps[]= {tmp1, tmp1};

  cout<<"tmp linha->"<<tmp.rows<<" tmp coluna->"<<tmp.cols<<endl;
  cout<<"tmp1 linha->"<<tmp1.rows<<" tmp1 coluna->"<<tmp1.cols<<endl;

  namedWindow("filtrada",WINDOW_NORMAL);
  sprintf( TrackbarName, "atenuação c %d",c_max );
  createTrackbar( TrackbarName, "filtrada", &c,c_max, on_trackbar_c );

  sprintf( TrackbarName, "Gama low %d",gama_l_max );
  createTrackbar( TrackbarName, "filtrada", &gama_l,gama_l_max, on_trackbar_gama_l );

  sprintf( TrackbarName, "Gama high %d",gama_h_max );
  createTrackbar( TrackbarName, "filtrada", &gama_h,gama_h_max, on_trackbar_gama_h );

  sprintf( TrackbarName, "frequencia de corte %d",D0_max );
  createTrackbar( TrackbarName, "filtrada", &D0,D0_max, on_trackbar_D0 );

  for(;;){
    cap >> image;
    cvtColor(image, imagegray, CV_BGR2GRAY);

    copyMakeBorder(imagegray, padded, 0, dft_M - image.rows, 0, dft_N - image.cols, BORDER_CONSTANT, Scalar::all(0));

    planos.clear();

    realInput = Mat_<float>(padded);
    Mat temporario = realInput.clone();
    normalize(temporario, temporario, 0, 1, CV_MINMAX);   //essa tres linhas são apenas
    namedWindow("original",WINDOW_NORMAL);                //para efeito de vizialização
    imshow("original", temporario);                       //da imagem original
    // insere as duas componentes no array de matrizes

    planos.push_back(realInput);
    planos.push_back(zeros);

    for(int i=0; i<dft_M; i++)
    {
      for(int j=0; j<dft_N; j++)
      {
          planos[0].at<float> (i,j) = log(planos[0].at<float> (i,j));
      }
    }

    merge(planos, complexImage);

    dft(complexImage, complexImage);

    deslocaDFT(complexImage);

    mulSpectrums(complexImage,filter,complexImage,0);

    deslocaDFT(complexImage);

    idft(complexImage, complexImage);

    planos.clear();

    split(complexImage, planos);

    normalize(planos[0], planos[0], 0, 1, CV_MINMAX);

    for(int i=0; i<dft_M; i++)
    {
      for(int j=0; j<dft_N; j++)
      {
            planos[0].at<float> (i,j) = exp(planos[0].at<float> (i,j));
      }
    }

    normalize(planos[0], planos[0], 0, 1, CV_MINMAX);
    namedWindow("filtrada",WINDOW_NORMAL);
    imshow("filtrada", planos[0]);

    waitKey(30);
  }

  return 0;
}
