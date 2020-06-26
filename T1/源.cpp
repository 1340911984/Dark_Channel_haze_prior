#include<opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"      
#include "opencv2/highgui/highgui.hpp"      
#include "opencv2/imgproc/imgproc.hpp"
#include<iostream>

using namespace cv;
using namespace std;
Mat MinFilter(Mat src, int kernel)

{
	int r = (kernel - 1) ;
	Mat dst_ex;
	Mat dst;
	dst.create(src.size(), src.type());
	copyMakeBorder(src, dst_ex, r, r, r, r, BORDER_CONSTANT, Scalar(255));

for(int k =0;k<3;k++)
	for (int i = r; i < dst_ex.rows - r; i++)
	{
		for (int j = r; j < (dst_ex.cols - r); j++)
		{
			int minVal = dst_ex.at<Vec3b>(i, j)[k];
			for (int s = -r; s < r + 1; s++)
			{
				for (int t = -r; t < r + 1; t++)
				{
					if (dst_ex.at<Vec3b>(i + s, j + t)[k] < minVal)
					{
						minVal = dst_ex.at<Vec3b>(i + s, j + t)[k];
					}
				}
			}
			dst.at<Vec3b>(i - r, j - r)[k] = minVal;
		}


	}
	return dst;
}

Mat DarkChannel(Mat image2, int nm ,int height , int width) {
	int r = (nm *nm) ;
	for(int kkk=0;kkk<3;kkk++)
		for (int h = nm/2  ; h < height - (nm / 2 )-1; h++) {
		for (int w = nm / 2 ; w < width - (nm / 2) - 1; w++) {
			vector<int> sum;
			for (int i = h-nm/2; i < h + nm / 2+1; i++)
				for (int j = w-nm/2; j < w + nm / 2+1; j++) {
					int temp = image2.at<Vec3b>(i,j)[kkk];
					sum.push_back(temp);
				}
			for (int i = 0; i < r; i++) {
				int temp = sum[i];
				if (image2.at<Vec3b>(h, w)[kkk] > temp) {
					image2.at<Vec3b>(h, w)[kkk] = temp;
				}
			}
		}
	}
	return image2;
}
double A = 0; //大气光成分
Mat image2;//用来操作的图
double tx = 255;//透射率

int main(int argc, char** argv) {

	Mat image = imread("C:\\Users\\86139\\Pictures\\Uplay\\timmm.png");//路径
	if (image.empty()) {
		printf("could not load image...\n");//没找到
		return -1;
	}
/*
	开始建立暗通道
	原理是取RBG中最小的值
	再最小值滤波
*/
	image2.create(image.size(), image.type());  //创建类型/大小一样的图片
	 int height = image.rows;                //获取图像的行
	int width = image.cols;                //获取图像的列
	int nc = image.channels();			  //通道
	imshow("input", image);

	for (int h = 0; h < height; h++)
		for (int w = 0; w < width; w++) {
				int	b = image.at<Vec3b>(h, w)[0];  //读取通道值
				int	g = image.at<Vec3b>(h, w)[1];
				int	r = image.at<Vec3b>(h, w)[2];
				int black = b;
				if (b > g) { black = g; }
				if (g > r) { black = r; }
				image2.at<Vec3b>(h, w)[0] = black;          //给新图像赋值
				image2.at<Vec3b>(h, w)[1] = black;		   
				image2.at<Vec3b>(h, w)[2] = black;	      
	}
	image2 = MinFilter(image2, 7);//最小值滤波后的暗通道
	Mat image4 = MinFilter(image, 7);//直接滤波后的原图
	//image2 = DarkChannel(image2, 5,height,width);
	//Mat image4 = DarkChannel(image, 5,height, width);
	imshow("output2", image2);
	imshow("output12", image4);
/*
	求大气光成分A
	原理就是取灰度值前0.1%的像素作为灰度值
*/
	/*
	double light = 0;
	for (int h = 0; h < height / 3; h++) 
		for (int w = 0; w < width; w++) {
			int	b2 = image2.at<Vec3b>(h, w)[0]; 
			int	g2 = image2.at<Vec3b>(h, w)[1];
			int	r2 = image2.at<Vec3b>(h, w)[2];
			double white2 = g2;
			if (b2 > g2) { white2 = b2; }
			if (r2 > b2) { white2 = r2; }
			double white_temp = (white2 / 255.0)*100;
			if (white_temp > light){
				light = white2;
				int	b = image.at<Vec3b>(h, w)[0];		
				int	g = image.at<Vec3b>(h, w)[1];
				int	r = image.at<Vec3b>(h, w)[2];
				double black = b;
				if (b > g) { black = g; }
				if (g > r) { black = r; }
				double white = g;
				if (b > g) { white = b; }
				if (g > b) { white = r; }
				A = white / 2 + black / 2;
			}
	}
	*/
	for (int h = 0; h < height / 3; h++)
		for (int w = 0; w < width; w++) {
				int	b = image.at<Vec3b>(h, w)[0];
				int	g = image.at<Vec3b>(h, w)[1];
				int	r = image.at<Vec3b>(h, w)[2];
				double black = b;
				if (b > g) { black = g; }
				if (g > r) { black = r; }
				double white = g;
				if (b > g) { white = b; }
				if (g > b) { white = r; }
				double A_temp = white / 2 + black / 2;
				if(A<A_temp)A = white / 2 + black / 2;
			}
/*
	求t(x）
	用已知的I(x)和A
	进行公式代换求解
*/
	
	printf("%f\n", A);
/*
	开始操作！	
*/
	A = 250;
	Mat image3;
	image3.create(image.size(), image.type());
		for (int h = 0; h < height; h++) 
			for (int w = 0; w < width; w++) {
				double	b = image.at<Vec3b>(h, w)[0];  //读取通道值
				double	g = image.at<Vec3b>(h, w)[1];
				double	r = image.at<Vec3b>(h, w)[2];
				double	tx = image4.at<Vec3b>(h, w)[0];
				for (int k = 0; k < 3; k++) {
					if (tx > image4.at<Vec3b>(h, w)[k])	tx = image4.at<Vec3b>(h, w)[k];
				}
				tx = 1 - 0.95*(tx / A);
				//cout << tx << endl;
				if (tx < 0.1)tx = 0.1;
				double b3 = (b - A) / tx + A;
				double g3 = (g - A) / tx + A;
				double r3 = (r - A) / tx + A;
				image3.at<Vec3b>(h, w)[0] = b3;          //给新图像赋值
				image3.at<Vec3b>(h, w)[1] = g3;
				image3.at<Vec3b>(h, w)[2] = r3;
			}
	imshow("output", image3);
	waitKey(0);
}
