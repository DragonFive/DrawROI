//#include "cv.h"
//#include "cvaux.h"
//#include "highgui.h"
//#include<string>
//#include"tinyxml.h"
//// for filelisting
//#include <stdio.h>
//#include <io.h>
//// for fileoutput
//#include <string>
//#include <fstream>
//#include <sstream>
//#include <iostream>

//
//using namespace std;


#pragma comment( lib, "vfw32.lib" ) 
#pragma comment( lib, "comctl32.lib" )
#include "mytools.h"
IplImage* image = 0;
IplImage* image2 = 0;
int start_roi = 0;
int roi_x0 = 0;
int roi_y0 = 0;
int roi_x1 = 0;
int roi_y1 = 0;
int numOfRec = 0;
char* window_name = "<SPACE>add <ENTER>save and load next <ESC>exit";

string IntToString(int num)
{
	ostringstream myStream; //creates an ostringstream object
	myStream << num << flush;
	/*
	* outputs the number into the string stream and then flushes
	* the buffer (makes sure the output is put into the stream)
	*/
	return(myStream.str()); //returns the string form of the stringstream object
};

void on_mouse(int event, int x, int y, int flag,void *)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		start_roi = 1;
		roi_x0 = x;
		roi_y0 = y;
	}
	if (event == CV_EVENT_MOUSEMOVE && flag == CV_EVENT_FLAG_LBUTTON)
	{
		roi_x1 = x;
		roi_y1 = y;

		//redraw ROI selection
		image2 = cvCloneImage(image);
		cvRectangle(image2, cvPoint(roi_x0, roi_y0), cvPoint(roi_x1, roi_y1), CV_RGB(255, 0, 255), 1);
		cvShowImage(window_name, image2);
		cvReleaseImage(&image2);
	}
	if (event == CV_EVENT_LBUTTONUP)
	{
		start_roi = 0;
	}
}
int test()


{
	IplImage * image1 = cvLoadImage("E:\\正样本\\19\\210035000\\0.jpg");
	//IplImage * image2 = cvCreateImage(cvSize(image1->width / 2, image1->width / 2), image1->depth, image1->nChannels);
	
	Mat matImg1=cv::cvarrToMat(image1);;
	//Mat matImg2(image2);
	int width1 = matImg1.size().width / 2;
	int height1 = matImg1.size().height / 2;
	Rect roi1(width1/2, height1/2, width1, height1);
	//Rect roi2(0, 0, width1, height1);
	Mat dest = matImg1(roi1);
	//matImg1(roi1).convertTo(dest, matImg1.type());
	imshow("呵呵", dest);
	imshow("哈哈", matImg1);
	waitKey(0);

	//*image2 =  dest;
	//cvShowImage("呵呵", image2);
	//cvWaitKey(0);
	return 0;

}
int main(int argc, char** argv)

{


	//初始化画框工具
	struct _finddata_t jpg_file;
	long hFile;
	int iKey = 0;


	string pathName = "rawdata";
	//初始化xml工具;
	string xmlFile = pathName;
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "");
	TiXmlElement * boatImg = new TiXmlElement("boat");
	string boatName, boatImo, boatMMSI,boatShipid;
	//刚开始要输入该船的MMSI/shipID/IMO和船的名字;
	cout << "请按提示输入:\n船的名字:";
	getline(cin, boatName);
	cout << "船的shipid:";
	cin >> boatShipid;
	cout << "船的IMO:";
	cin >> boatImo;
	cout << "船的MMSI:";
	cin >> boatMMSI;

	pathName = get_direc_path();
	//pathName = boatMMSI;
	//开始写xml的开头;
	boatImg->SetAttribute("name", boatName.c_str());
	boatImg->SetAttribute("shipid", boatShipid.c_str());
	boatImg->SetAttribute("imo", boatImo.c_str());
	boatImg->SetAttribute("mmsi", boatMMSI.c_str());
	

	//	get *.jpg files in directory
	if ((hFile = _findfirst((pathName+"\\*.jpg").c_str(), &jpg_file)) == -1L)
		printf("no *.jpg files in directory '%s'\n",pathName);
	else
	{

		//	init highgui
		cvAddSearchPath(pathName);
		cvNamedWindow(window_name, 1);
		cvSetMouseCallback(window_name, on_mouse);

		//	init output of rectangles to the info file
		ofstream output("info.txt");

		string strPrefix;
		string strPostfix;

		//	open every *.jpg file
		do
		{
			

			//开始画框;
			numOfRec = 0;
			strPostfix = "";
			strPrefix = pathName+"\\";
			strPrefix += jpg_file.name;
			printf("%s\n", jpg_file.name);
			image = cvLoadImage(strPrefix.c_str());
			
			if (image==NULL)
			{
				cout << "加载图像失败" << endl;
			}
			//	work on current image
			string tempPos[100];

			do
			{

				cvShowImage(window_name, image);

				// used cvWaitKey returns:
				//	<Enter>=13		save added rectangles and show next image
				//	<ESC>=27		exit program
				//	<Space>=32		add rectangle to current image
				//  any other key clears rectangle drawing only
				iKey = cvWaitKey(0);
				switch (iKey)
				{
				case 27:
					cvReleaseImage(&image);
					cvDestroyWindow(window_name);
					return 0;
				case 32:
					numOfRec++;
					// currently two draw directions possible:
					//		from top left to bottom right or vice versa
					int xzuobiao, yzuobiao, kuan, gao;
					if (roi_x0<roi_x1 && roi_y0<roi_y1)
					{
						printf("   %d. rect x=%d\ty=%d\twidth=%d\theight=%d\n", numOfRec, roi_x0, roi_y0, roi_x1 - roi_x0, roi_y1 - roi_y0);
						// append rectangle coord to previous line content
						strPostfix += " " + IntToString(roi_x0) + " " + IntToString(roi_y0) + " " + IntToString(roi_x1 - roi_x0) + " " + IntToString(roi_y1 - roi_y0);
						xzuobiao = roi_x0; yzuobiao = roi_y0; kuan = roi_x1 - roi_x0; gao = roi_y1 - roi_y0;
					}
					if (roi_x0>roi_x1 && roi_y0>roi_y1)
					{
						printf("   %d. rect x=%d\ty=%d\twidth=%d\theight=%d\n", numOfRec, roi_x1, roi_y1, roi_x0 - roi_x1, roi_y0 - roi_y1);
						// append rectangle coord to previous line content
						strPostfix += " " + IntToString(roi_x1) + " " + IntToString(roi_y1) + " " + IntToString(roi_x0 - roi_x1) + " " + IntToString(roi_y0 - roi_y1);
						xzuobiao = roi_x1; yzuobiao = roi_y1; kuan = roi_x0 - roi_x1; gao = roi_y0 - roi_y1;

					}
					tempPos[(numOfRec - 1) * 4] = IntToString(xzuobiao); tempPos[(numOfRec - 1) * 4 + 1] = IntToString(yzuobiao);
					tempPos[(numOfRec - 1) * 4 + 2] = IntToString(kuan); tempPos[(numOfRec - 1) * 4 + 3] = IntToString(gao);
					//先复制一份原来的image
					//IplImage * copyed = cvCreateImage(cvSize(image->width, image->width), image->depth, image->nChannels);
					//cvCopy(image, copyed, NULL);

					//下面把选的区域保存下来;
					Mat matImg=cv::cvarrToMat(image); //(image, 0);
					Rect cutedBoat(xzuobiao, yzuobiao, kuan, gao);
					Mat dest = matImg(cutedBoat);

					
					//CvSize size = cvSize(kuan, gao);//区域大小
					//cvSetImageROI(copyed, cvRect(xzuobiao, yzuobiao, size.width, size.height));//设置源图像ROI


					//IplImage* pDest = cvCreateImage(size, copyed->depth, copyed->nChannels);//创建目标图像
					//cvCopy(copyed, pDest); //复制图像
					//cvResetImageROI(pDest);//源图像用完后，清空ROI

					char afterStr[12];
					memset(afterStr, 0, sizeof(afterStr) / sizeof(char));
					//cout << "hear-" << endl;
					string preStr = "afterCut\\" + boatMMSI;
					sprintf(afterStr, "_%d_", numOfRec);
					string afterCutPath = preStr + afterStr + jpg_file.name;
					//cout << afterCutPath << endl;
					IplImage tosave = dest;
					cvSaveImage(afterCutPath.c_str(), &tosave);//保存目标图像
					
					break;
				}
			} while (iKey != 13);

			// save to info file as later used for HaarTraining:
			//	<rel_path>\jpg_file.name numOfRec x0 y0 width0 height0 x1 y1 width1 height1...
			if (numOfRec>0 && iKey == 13)
			{
				
				//对每个图片加上下面的一个节点
				TiXmlElement * photoElm = new TiXmlElement("photo");
				photoElm->SetAttribute("name", jpg_file.name);
				photoElm->SetAttribute("number", numOfRec);//添加一个number属性;
				
				for (int i = 0; i < numOfRec; i++)
				{
					TiXmlElement * zonePos = new TiXmlElement("zone");
					zonePos->SetAttribute("xZuobiao", tempPos[i * 4].c_str());
					zonePos->SetAttribute("yZuobiao", tempPos[i * 4+1].c_str());
					TiXmlElement * widthElement = new TiXmlElement("width");
					widthElement->LinkEndChild(new TiXmlText(tempPos[i * 4 + 2].c_str()));
					TiXmlElement * heightElement = new TiXmlElement("height");
					heightElement->LinkEndChild(new TiXmlText(tempPos[i * 4 + 3].c_str()));
					zonePos->LinkEndChild(widthElement);
					zonePos->LinkEndChild(heightElement);

					photoElm->LinkEndChild(zonePos);

				}
				boatImg->LinkEndChild(photoElm);

				//写入info.txt文件里
				//output << strPrefix << " " << numOfRec << strPostfix << "\n";
			}
			

			cvReleaseImage(&image);
		} while (_findnext(hFile, &jpg_file) == 0);


		output.close();
		cvDestroyWindow(window_name);
		_findclose(hFile);
	}
	doc.LinkEndChild(decl);
	doc.LinkEndChild(boatImg);
	doc.SaveFile(("xmlFIles/" + boatMMSI + ".xml").c_str());
	return 0;
}