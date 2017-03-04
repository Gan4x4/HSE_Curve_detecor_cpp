#ifndef CONTUR_H
#define CONTUR_H

#include <cv.h>
#include <highgui.h>
#include <vector>
#include <set>
#include <iostream>
#include <algorithm>
#include <math.h>

#define MAX_POINTS 10000

using namespace std;

typedef vector<vector<int>> Matrix ;
typedef vector < vector < pair<int,int> > > List;
typedef pair<int, int> Point; 

class Color {
public: 	
	uchar R;
	uchar G;
	uchar B;
	uchar A;
	//uchar br255;
	
	Color(uchar r, uchar g, uchar b, uchar a) : R(r),G(g),B(b),A(a){};
	Color() : R(0),G(0),B(0),A(0){};
	bool operator== (Color &c)
	{
		if (c.R == R && c.B==B && c.G == G && c.A==A) return true;
		return false;
	}
	Color(IplImage* img,int x, int y){
		int tmp =IPL_DEPTH_8U;
		uchar* ptr = (uchar*) (img->imageData + y * img->widthStep);
		if (img->depth == IPL_DEPTH_8U)
		{
			// Получаем указатель на начало строки 'y'
			R=ptr[3*x+2];
			G=0;
			B=0;
			A=0;
			G=ptr[3*x+1];
			B=ptr[3*x];
			
		};

		/*
		R=ptr[3*x+1];
			G=ptr[3*x+2];
			B=ptr[3*x+3];
			A=0;

		*/




		
	}
}; 


class Node{
public:
	int n;
	int x;
	int y;
	//Color color;
	Node(int n,int x, int y) : n(n), x(x), y(y) {};
	Node(int n, Point p) : n(n), x(p.first), y(p.second) {};
	
	bool operator == (const Node &n){ return (n.x==x && n.y==y); 	};
	bool operator < (const Node &n){ 
		if (n.x < x ) return true; 	
		if (n.x == x) return (n.y < y);
		return false;
	}

	Point point(void)
	{
		return Point(x,y);
	}

};

class Contur  {

private:
	Matrix m1;
	List l1;
	Color bg_color, object_color,fill_color,work_color;
	CvGraph *graph;
	vector <Node> nodes;
	int color_depth;
	int xmin,xmax,ymin,ymax;
	int init_x,init_y;
	int old_x,old_y;
	Point massCenter;
	IplImage* our_image;
	List CreateMST(List);
	vector<int> Find_Shortest_Paths(List g,int start,int end);
	List MST;
	Point Bad_Point;

public:
	Point t;
	Contur()
	{
		object_color=Color(255,255,255,0);
		fill_color=Color(35,0,255,0);
		work_color=Color(0,255,0,0);
		Bad_Point=Point(-1,-1);
	}

	void Contur_One(IplImage* img,int init_x, int init_y)
	{
		our_image=img;
		this->init_x=init_x;
		this->init_y=init_y;
		//work_color=Color(0,0,0,0);
		
		//graph=cvCreateGraph( CV_SEQ_KIND_GRAPH,sizeof(CvGraph), sizeof(CvGraphVtx)+sizeof(int)*2,sizeof(CV_WEIGHTED_EDGE), storage);
		color_depth = img->depth;
		
		unsigned int n=0;
		xmin=0,xmax=0,ymin=0,ymax=0;
		int x=init_x;
		int y=init_y;
		if (! (Color(img,x,y) == object_color) )
		{
			cout << "Bad start point" << "\n";
			return;
		};
		
		Point P(x,y);
		Point Prev_P=P;
		vector<Point> p_start_points;
		int i=0;
		
		do{
			if (P==getStartPoint() && Prev_P !=P) p_start_points.push_back(Prev_P); 
			if (! this->in_list(P.first,P.second))
				{
					nodes.push_back(Node(n,P));
					n++;
				};
			Find_next_node(img,P,Prev_P);
			
			if (i> 40000) {
				cout << "Break \n";
				break;
			}
			
			
		}while ( Stop_Criterion(P,Prev_P,p_start_points) );

	};

	Point GetFirstPoint(IplImage* img,Point a,Point b)
	{

		IplImage *dst = cvCreateImage( cvSize( img->width+2, img->height+2 ),img->depth, img->nChannels );
		cvZero(dst);
		fill(dst,Point(0,0),fill_color);
		IplImage *img2 = cvCreateImage( cvSize( img->width, img->height ),img->depth, img->nChannels );
		cvSetImageROI(img2, cvRect(0,0,img->width,img->height));
		cvCopyImage(img,img2);
		cvResetImageROI(img2);
		cvSetImageROI(dst, cvRect(1,1,img2->width,img2->height));
		cvCopyImage(img2,dst);
		cvResetImageROI(dst);
		fill(dst,b,fill_color);
		
		Point res=Bad_Point;
		//int sd[8][2] = {{-1,0},{1,0},{0,-1},{0,1},{-1,1},{-1,-1},{1,-1},{1,1}};
		int sd[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
		for (int i=0;i<4;i++)
		{
			
			int nx=1+a.first+sd[i][0];
			int ny=1+a.second+sd[i][1];
			
					if ( Color(dst,nx,ny) == fill_color ) 
					{
						res=Point(nx-1,ny-1);
						SetPixel(dst,Point(nx,ny),Color(200,55,7,0));
						break;
					}
					else SetPixel(dst,Point(nx,ny),work_color);
						
				//	} 
		//	}
		//	if (res != Bad_Point) break;
		
		}
		
		//cvNamedWindow("tmp", 0);
		//cvShowImage("tmp", dst);
		cvReleaseImage( &dst );
		cvReleaseImage( &img2 );
		//cout << " First search point " << res.first << " , " << res.second << "\n";
		return res;
	}

	vector<Node> Contur_Two(IplImage* img,Point bg,Point ob_color)
	{
		//GetFirstPoint(img,ob_color,bg);
		vector<Node> res;
		unsigned int n=0;
		//Point P(x,y);
		Point Prev_P=GetFirstPoint(img,ob_color,bg);
		if (Prev_P == Bad_Point) {
			cout << "Bad start point \n";
			return res;
		};
		t=Prev_P;
		Point P = ob_color;
		vector<Point> Jacob;
		//vector<Point> p_start_points;
		int i=0;
		bool exit = false;
		
		do{
			//if (P==getStartPoint() && Prev_P !=P) p_start_points.push_back(Prev_P); 
			vector<Node>::iterator it=find(res.begin(),res.end(),Node(0,P));

			if (it == res.end())
				{
					res.push_back(Node(n,P));
					n++;
				}
			else{
				
				//Prev_P=res.back().point();
				while(res.back().point() !=P)
				{
					res.pop_back();
					n--;
				}
				//Prev_P=res.back().point();
				
				
				//P=res.back().point();
				//n--;

			};
			
			Find_next_node(img,P,Prev_P);
			if (P==ob_color)
			{
				if (Jacob.end() != find(Jacob.begin(),Jacob.end(),Prev_P)) exit = true;
				Jacob.push_back(Prev_P);
			}
			//if (i> 172) {
			//	cout << "Break1 \n";
				//break;
			//}
			
			if (i> MAX_POINTS) {
				cout << "Break2 \n";
				break;
			}
			i++;

			//if ( P == ob_color && res.size()>2 ) exit = true; // First point reached
			//if (Prev_P==t && res.size()>1) exit = true; // Not a contur
			//if ( P == ob_color && Prev_P==t && res.size()>1 ) exit = true;

			
		}while ( ! exit);
		//cout << i << "Iteration \n";
		return res;

	}


	void setBgColor(IplImage* img,int x,int y)
	{
		bg_color=Color(img,x,y);
	};

	void setObjectColor(IplImage* img,int x,int y)
	{
		object_color=Color(img,x,y);
	};


	int Weight(IplImage* img,Node *a,Node *b)
	{
		// By Color
		//fill(img, cvPoint(1, 1));
		//img = cvLoadImage("fill1.bmp",1);
		Color c(img,b->x,b->y);
		Color c3(img,1,1);
		Color c4(img,31,10);
		int res=0;
		for(int i=-1;i<2;i++)
			{
				for(int j=-1;j<2;j++)
				{
					Color c1= Color(img,b->x+i,b->y+j);
					
					if ( c1 == fill_color ) res++;
				}
		}
		return res;
	};


	void Get_Edges(IplImage* img)
	{
		l1.resize(nodes.size());
		for (int k=0;k<nodes.size()-1;k++)
		{
			for(int i=-1;i<2;i++)
			{
				for(int j=-1;j<2;j++)
				{
					if (in_list(nodes[k].x+i,nodes[k].y+j))
					{
						
						vector <Node>::iterator it = find(nodes.begin(),nodes.end(),Node(0,nodes[k].x+i,nodes[k].y+j));
					//	if (it !=nodes.end() )
					//	{
						int n=it->n;
						
						//if ((k == 0 )&& (n == getEndNode().n)) continue;
						//int wt=-1*Weight(img,&nodes[k],&nodes[n]);
						int wt=-1*Weight(img,&nodes[k],&nodes[n]);
						// This condition for full conturs onle
						if(wt==0 || wt ==-7 || n==k) continue;
						l1[k].push_back(pair<int,int>(it->n,wt));
						
					//	else
					//	{
					//		cout <<nodes[k].n << "\n";
					//	}
					}
				}
			}
		}
		cout << "Three" <<l1.size();
		//set < pair<int,int> > p = CreateMST(l1);
		//vector<int> p=Find_Shortest_Paths(l1,0,getEndNode().n);
		MST=CreateMST(l1);
		//cout << p.size();
	}

	Node getEndNode()
	{
		return nodes[nodes.size()-1];
	}

	Point getStartPoint()
	{
		if (nodes.size() > 1) return nodes[0].point();
		return Point(-1,-1);

	}

	Point getCenter()
	{
		
	int n=nodes.size();
	    double xm=0, ym=0;
		for(int i=0; i<nodes.size(); i++)
		{
			xm+=nodes[i].x;
			ym+=nodes[i].y;
		}
		xm/=n; ym/=n; // координаты точки внутри многоугольника
		double s = 0;
		double xc=0,yc=0;
		// Шагаем по треугольникам. Их n штук
		for(int i=0; i<n; i++)
		{
			// используем полученные формулы (**)
			double s1 = 0.5*fabs((nodes[i].x-nodes[(i+1)%n].x)*(ym-nodes[(i+1)%n].y) - (xm-nodes[(i+1)%n].x)*(nodes[i].y-nodes[(i+1)%n].y));
			xc+=s1*(xm+nodes[i].x+nodes[(i+1)%n].x)/3;
			yc+=s1*(ym+nodes[i].y+nodes[(i+1)%n].y)/3;
			s+=s1;
		}
		xc/=s; yc/=s;
		return Point((int)xc,(int)yc);
	}

	bool Stop_Criterion(const Point& curr, const Point& prev,vector<Point> list)
	{
		//Jacob's stopping criterion.
		if (( nodes[0].x ==  curr.first )&& (nodes[0].y == curr.second ) )
		{
	//		cout << nodes[0].x << " == " << curr.first << " ;  " << nodes[0].y << " ==" << curr.second << "\n";
			if (list.end() != find(list.begin(),list.end(),prev))
			{
					return false;
			};

		};
		return true;
	};

	bool in_list(int x, int y)
		{
			Node n(0,x,y);
			vector<Node>::iterator it;
			it = find(nodes.begin(),nodes.end(),n);
			return (!( it == nodes.end()));
		}

	
	bool Find_next_node(IplImage* img, Point& around  ,Point& from)
	{
		int weight=0;
		int sd[8][2] = {{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1}};
		

		int start = 0;

		if (from != around)
		{
			//for (int j=0;j<8;j++) if ( (x+sd[j][0])==old_x && (y+sd[j][1])==old_y) start=j;
			for (int j=0;j<8;j++) 
				if ( (around.first+sd[j][0])==from.first && (around.second+sd[j][1])==from.second)
					{
						start=j+1;
				}
		};
		
			for (int j=0;j<8;j++)
			{
				int ind = start+j;
				if (ind>=8) ind=ind -8;
				int nx=around.first+sd[ind][0];
				int ny=around.second+sd[ind][1];
				if (nx>=0 && ny>=0 && nx<img->width && ny< img->height )
					{
						if ( Color(img,nx,ny) == object_color ) 
						{
							// Create Edge
							around=Point(nx,ny);
							return true;
						}
						from=Point(nx,ny);
					} 
			}
		
		return false;
	}
	

	unsigned int Nodes_count()
	{
		return nodes.size();
	};

	int get_Width()
	{
		return xmax - xmin;
	};

	int get_Heigth()
	{
		
		return ymax -ymin;
	};

	void SetPixel(IplImage* img,Point P,Color C)
	{
		//if (color_depth == img->depth)
		//{
			uchar* ptr = (uchar*) (img->imageData + P.second*img->widthStep);
			ptr[3*P.first+2] = C.R;
			ptr[3*P.first+1] = C.G;
			ptr[3*P.first] = C.B;
		//};
	}


void fill(IplImage* src, Point p,Color fill  )
{
		//cvPoint seed=cvPoint(p.first, p.second);
		//CvScalar color=CV_RGB(fill_color.R,fill_color.G, fill_color.B);
	CvScalar color=CV_RGB(fill.R,fill.G,fill.B);
            
		assert(src);

        CvConnectedComp comp;

        cvFloodFill( src, cvPoint(p.first, p.second), color, 
                 cvScalarAll(0), // минимальная разность
                 cvScalarAll(0), // максимальная разность
                 &comp,
				 4, // Связанность
                 //CV_FLOODFILL_FIXED_RANGE + 4,
                 NULL);
		//fill_color=Color(src,1,1);
        // покажем площадь заливки
        //printf("[filled area] %.2f\n", comp.area);
		//Color c(src,28,10);
		//Color c3(src,1,1);
		//Color c4(src,31,10);
		//cout << "n";
}


	void Show(IplImage* img, vector <Node> contur)
	{
		//if (color_depth == img->depth)
		//{
			unsigned int i=0;
			Color fc=fill_color;
				//Color(0,255,0,0);
			//float step=255/contur.size();
			//float value=step;
			while(i < contur.size())
			{
				//fc=Color(floor(value),255,floor(value),0);
				SetPixel(img,contur[i].point(),fc);
				i++;
				//value +=step;
			}
			//SetPixel(img,Point(init_x,init_y),Color(128,0,0,0));
			//SetPixel(img,getCenter(),Color(128,0,0,0));
		//}
		
	}

	void ShowThree(IplImage* img,int start = 0)
	{

			unsigned int i=0;
			List three = MST;
			Color c2=Color(0,0,0,0);
			Node n0 = nodes[start];
			vector<int> to_visit;
			vector<int> visited;
			to_visit.push_back(start);
			int j=0;
			while(! to_visit.empty())
			{

				Node current_node = nodes[to_visit.back()];
				SetPixel(img,current_node.point(),c2);
				to_visit.pop_back();
				visited.push_back(current_node.n);
				for(int i =0;i<three[current_node.n].size();i++)
				{
					vector<int>::iterator it;
					it=find(visited.begin(),visited.end(),three[current_node.n][i].first);
					if (it == visited.end()) to_visit.push_back(three[current_node.n][i].first);
				}
				j++;
			}
			//SetPixel(img,Point(init_x,init_y),Color(128,0,0,0));
			
	
		


	}
	
};

#endif
