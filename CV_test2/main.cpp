#include <cv.h>
#include "highgui.h"
#include "contur.h"
#include <iostream>
#include <fstream>

int mClick=0;
Point bg,object;
vector<vector<Node>> All_Conturs;
int conturs_count = 0;
ofstream log1("finded_conturs.log");

int find_contur(vector<Node> v)
{
	vector<Node> etalon;
	bool eq=false;
	for(int i=0;i<All_Conturs.size();i++)
	{
		if (v.size() == All_Conturs[i].size())
		{
			for(int j=0;j<All_Conturs[i].size();j++)
			{
				if (v[j].point()==All_Conturs[i][0].point())
				{
					//int m=0;
					for(int k=j;k<v.size();k++) {
						etalon.push_back(v[k]);
						
					};

					for(int k=0;k<j;k++) {
						etalon.push_back(v[k]);
						
					};
					break;
				}
			}
			eq=true;
			for(int h=0;h<etalon.size();h++)
			{
				
				if (etalon[h].point() != All_Conturs[i][h].point()) eq=false;
			}
			if (eq==true) 
			{
				cout << "This contur equal to contur " << i << " in unique countur list. \n";
				return i;
			}
		};

	}
	All_Conturs.push_back(v);
	return -1;
}



void makeContur(IplImage* img,Point bg,Point object)
{
	

	//Contur::GetFirstPoint(img,bg);
/*
	IplImage *dst = cvCreateImage( cvSize( img->width+2, img->height+2 ),
                               img->depth, img->nChannels );
	cvZero(dst);
	IplImage *img2 = cvCreateImage( cvSize( img->width, img->height ),
                               img->depth, img->nChannels );
	cvSetImageROI(img2, cvRect(0,0,img->width,img->height));
	cvCopyImage(img,img2);
	cvResetImageROI(img2);
		//IplImage* dst = cvCloneImage(image);
		//cvSetImageROI(img, cvRect(0,0,img->width,img->height));
		//cvSetImageROI(dst, cvRect(1,1,dst->width-1,dst->height-1));
	cvSetImageROI(dst, cvRect(1,1,img2->width,img2->height));
		//cvSetImageROI(img, cvRect(5,5,15,15));
		//cvZero(dst);
		cvCopyImage(img2,dst);
		//cvResetImageROI(img);
		cvResetImageROI(dst);
		cvNamedWindow("tmp", 0);
		cvShowImage("tmp", dst);
		cvReleaseImage( &dst );
		cvReleaseImage( &img2 );
*/
	Contur cn=Contur();
	vector<Node> cx = cn.Contur_Two(img,bg,object);
	if (cx.size()<2) {cout << "Can't build the contur \n";}
	else
	{

	// cx=cn.Contur_Two(image,28,10);
	IplImage* result = cvCloneImage(img);

	//IplImage* result = cvCreateImage(cvSize(img->width, img->height), 8, 3);
	cn.Show(result,cx);
	
	log1 << "Contur #" << conturs_count << "  ";
	log1 << " has " << cx.size() << " points; " ;
	log1 << "start point " << cx[0].x << "," << cx[0].y <<"\n";


	cout << "Contur #" << conturs_count << "  ";
	cout << " has " << cx.size() << " points; " ;
	cout << "start point " << cx[0].x << "," << cx[0].y <<"\n";
	conturs_count++;
	find_contur(cx) ;

	for(int i=0;i<cx.size();i++)
	{
		log1 << cx[i].x << ";" << cx[i].y << ";\n";
	};

	cn.SetPixel(result,object,Color(128,0,0,0));
	//cn.SetPixel(result,cn.t,Color(128,128,0,0));
	cvNamedWindow("Result", 0);
	cvShowImage("Result", result );
	cvWaitKey(0);
	cvReleaseImage( &result );
	};


}
void myMouseCallback( int event, int x, int y, int flags, void* param )
{
	IplImage* img = (IplImage*) param;

	switch( event ){
		case CV_EVENT_MOUSEMOVE: 
			break;

		case CV_EVENT_LBUTTONDOWN:
			//printf("%d x %d\n", x, y);

			// вызываем нашу функцию-обЄртку вокруг cvFloodFill()
			//fill(img, cvPoint(x, y));
			if (mClick==0)
			{
				bg=Point(x,y);
				cout << "Select a start point.\n";
			};
			if (mClick==1)
			{
				object=Point(x,y);
				if (Color(img,bg.first,bg.second) == Color(img,object.first,object.second)) {cout << "Bad start point \n";}
				else makeContur(img,bg,object);
				mClick=0;
				//cout << "Press Space to make a contur.\n";
			};

			mClick++;
			
			break;

		case CV_EVENT_LBUTTONUP:
			break;
	}
}





int main( int argc, char** argv ) 
{
	if (argc != 2) cout << "File name not specified, try to load plane.bmp";
    char* filename = argc == 2 ? argv[1] : "plane.bmp";
    
	// задаЄм высоту и ширину картинки
        //int height = 620;
        //int width = 440;
        // задаЄм точку дл€ вывода текста
        //CvPoint pt = cvPoint( height/4, width/2 );
        // —оздаЄи 8-битную, 3-канальную картинку
        //IplImage* hw = cvCreateImage(cvSize(height, width), 8, 3);
		IplImage* image = cvLoadImage(filename,1);

        // заливаем картинку чЄрным цветом
        //cvSet(hw,cvScalar(0,0,0));
        // инициализаци€ шрифта
        //CvFont font;
        //cvInitFont( &font, CV_FONT_HERSHEY_COMPLEX,1.0, 1.0, 0, 1, CV_AA);
        // использу€ шрифт выводим на картинку текст
        //cvPutText(hw, "OpenCV Step By Step", pt, &font, CV_RGB(150, 0, 150) );

        // создаЄм окошко
        //cvNamedWindow("Hello World", CV_WINDOW_AUTOSIZE);
		if (image !=NULL)
		{
		cvNamedWindow("Initial Image", 0);
		 // вешаем обработчик мышки
		cvShowImage("Initial Image", image);
        cvSetMouseCallback( "Initial Image", myMouseCallback, (void*) image);
		cout << "Select a background color \n";
		cvWaitKey(0);
		/*
		char c;
		do{
			c = cvWaitKey(0);
			cout << c;
		}while (c!=27);
		*/
		
        // показываем картинку в созданном окне
        
		//cn.fill(image, cvPoint(1, 1));
		
		
		//cvSaveImage("fill1.bmp",image);
        // ждЄм нажати€ клавиши
		/*
        cvWaitKey(0);
//		Contur cn(image,28,10);
		cvNamedWindow("Result Three", 0);
		IplImage* image1 = cvCreateImage(cvSize(image->width, image->height), 8, 3);
		//vector <Node> cx=cn.Contur_Two(image,28,10);
		//cn.Show(image1,cx);
		cvShowImage("Result Three", image1);
		cvWaitKey(0);

		//cn.Get_Edges(image);
		//cn.ShowThree(image1,171);
		//cn.Show(image1);
		cvShowImage("Result Three", image1);
		cout << "Graph size" << cn.Nodes_count();
        cvWaitKey(0);
		*/
        // освобождаем ресурсы
		log1.close();
        cvReleaseImage(&image);
		//cvReleaseImage(&image1);
        cvDestroyWindow("Initial Image");
		//cvDestroyWindow("Result Three");
	}
	else {
		cout << " file " << filename << " not found. \n";
		system("pause");
	};
    return 0;
}