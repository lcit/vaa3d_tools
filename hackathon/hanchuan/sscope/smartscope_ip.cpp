/* smartscope_ip.cpp
 * adapted from Hanchuan's region growing code
 * 2010-04-02: create this program by Yang Yu
 * 2011-12-21: create this program by Jianlong Zhou
 */

#include <QtGui>

#include <cmath>
#include <stdlib.h>
#include <ctime>

#include <vector>

#include <sstream>
#include <iostream>

#include "smartscope_ip.h"
#include "smartscope_gui.h"

#include <basic_surf_objs.h>
#include <volimg_proc.h>
#include <img_definition.h>
#include <basic_landmark.h>



RgnGrow3dClass::RgnGrow3dClass()
{
	ImgWid = 0, ImgHei = 0, ImgDep = 0;
	quantImg1d=0; quantImg3d=0;
	PHCDONEIMG3d = 0, PHCDONEIMG1d = 0;
	
	STACKCNT = -1, MAXSTACKSIZE = 16, IFINCREASELABEL=-1, PHCURLABEL=-1;
	PHCLABELSTACK3d = 0, PHCLABELSTACK1d = 0;
	PHCLABELSTACKPOS = 0;
	
	PHCURGNPOS = 0, PHCURGNPOS_head = 0;
	PHCURGN = 0, PHCURGN_head = 0;
	TOTALPOSnum = 0, TOTALRGNnum = 0;
}

RgnGrow3dClass::~RgnGrow3dClass()
{
	deleteIntImage3dPairMatlabProtocal(quantImg3d,quantImg1d);
	deleteIntImage3dPairMatlabProtocal(PHCLABELSTACK3d,PHCLABELSTACK1d);
	deleteIntImage3dPairMatlabProtocal(PHCDONEIMG3d,PHCDONEIMG1d);
	
	ImgWid = 0, ImgHei = 0, ImgDep = 0;
	
	STACKCNT = -1, MAXSTACKSIZE = 16, IFINCREASELABEL=-1, PHCURLABEL=-1;
	PHCLABELSTACKPOS = 0;
	
	PHCURGN = PHCURGN_head;
	for(V3DLONG i=0;i<TOTALRGNnum;i++)
	{
		RGN * pnextRgn = 0;
		if (PHCURGN)
		{
			pnextRgn = PHCURGN->next;
			PHCURGNPOS = PHCURGN->poslist;
			for(V3DLONG j=0;j<PHCURGN->poslistlen;j++)
			{
				POS *pnextPos = 0;
				if (PHCURGNPOS) 
				{
					pnextPos = PHCURGNPOS->next;
					delete PHCURGNPOS;
				}
				PHCURGNPOS = pnextPos;
			}
			delete PHCURGN;
		}
		PHCURGN = pnextRgn;
	}
	TOTALPOSnum = 0, TOTALRGNnum = 0;
}



// RegionGrowing Class
void RegionGrowing::rgnfindsub(int rowi,int colj, int depk, int direction,int stackinc, RgnGrow3dClass * pRgnGrow)
{
	if (pRgnGrow->STACKCNT >= pRgnGrow->MAXSTACKSIZE)
	{
		if (pRgnGrow->IFINCREASELABEL != 0)
			pRgnGrow->IFINCREASELABEL = 0;
		return;
	}
	
	BYTE *** flagImg = pRgnGrow->PHCDONEIMG3d;
	int ImgWid = pRgnGrow->ImgWid;
	int ImgHei = pRgnGrow->ImgHei;
	int ImgDep = pRgnGrow->ImgDep;
	
	if (stackinc==1)
	{
		//printf("STACKCNT=%i PHCLABELSTACKPOS=%i\n",STACKCNT,PHCLABELSTACKPOS);
		pRgnGrow->PHCLABELSTACK3d[0][0][pRgnGrow->PHCLABELSTACKPOS] = depk;
		pRgnGrow->PHCLABELSTACK3d[0][1][pRgnGrow->PHCLABELSTACKPOS] = colj;
		pRgnGrow->PHCLABELSTACK3d[0][2][pRgnGrow->PHCLABELSTACKPOS] = rowi;
		
		pRgnGrow->STACKCNT++;
		pRgnGrow->PHCLABELSTACKPOS++;
		
		flagImg[depk][colj][rowi] = -1;//pRgnGrow->PHCDONEIMG2d[coli][rowj] = -1;
		//PHCLABELIMG[coli][rowj] = PHCURLABEL;
		
		//set the current pos location and return the 
		if (pRgnGrow->PHCURGNPOS)
		{
			pRgnGrow->PHCURGNPOS->pos = (V3DLONG) depk*(pRgnGrow->ImgHei * pRgnGrow->ImgWid) + colj*(pRgnGrow->ImgWid) + rowi; //
			pRgnGrow->PHCURGNPOS->next = new POS;
			if (pRgnGrow->PHCURGNPOS->next==0)
			{printf("Fail to do: pRgnGrow->PHCURGNPOS->next = new POS; -->current phcDebugPosNum=%i.\n",phcDebugPosNum);}
			pRgnGrow->PHCURGNPOS = pRgnGrow->PHCURGNPOS->next;
			pRgnGrow->TOTALPOSnum++;
			//printf("current pixel: row=%i col=%i total_pxl_visted=%i \n",rowj,coli,pRgnGrow->TOTALPOSnum);
		}
		else
		{
			printf("PHCURGNPOS is null!!\n");
		}
	}
	else //%if stackinc==0,
	{
		//pRgnGrow->PHCDONEIMG[coli][rowj] = -2;
		flagImg[depk][colj][rowi] = -2;
	}
	
	// % search 26 direction orders
	
	// 1
	if (rowi>0 && flagImg[depk][colj][rowi-1]==1)
		rgnfindsub(rowi-1,colj,depk,1,1,pRgnGrow); 
	// 2
	if (rowi<ImgWid-1 && flagImg[depk][colj][rowi+1]==1)
		rgnfindsub(rowi+1,colj,depk,1,1,pRgnGrow); 
	// 3
	if (colj>0 && flagImg[depk][colj-1][rowi]==1)
		rgnfindsub(rowi,colj-1,depk,1,1,pRgnGrow); 
	// 4
	if (colj<ImgHei-1 && flagImg[depk][colj+1][rowi]==1)
		rgnfindsub(rowi,colj+1,depk,1,1,pRgnGrow); 
	// 5
	if (depk>0 && flagImg[depk-1][colj][rowi]==1)
		rgnfindsub(rowi,colj,depk-1,1,1,pRgnGrow); 
	// 6
	if (depk<ImgDep-1 && flagImg[depk+1][colj][rowi]==1)
		rgnfindsub(rowi,colj,depk+1,1,1,pRgnGrow); 
	// 7
	if (rowi>0 && colj>0 && flagImg[depk][colj-1][rowi-1]==1)
		rgnfindsub(rowi-1,colj-1,depk,1,1,pRgnGrow); 
	// 8
	if (rowi<ImgWid-1 && colj>0 && flagImg[depk][colj-1][rowi+1]==1)
		rgnfindsub(rowi+1,colj-1,depk,1,1,pRgnGrow); 
	// 9
	if (rowi>0 && colj<ImgHei-1 && flagImg[depk][colj+1][rowi-1]==1)
		rgnfindsub(rowi-1,colj+1,depk,1,1,pRgnGrow); 
	// 10
	if (rowi>ImgWid && colj<ImgHei-1 && flagImg[depk][colj+1][rowi+1]==1)
		rgnfindsub(rowi+1,colj+1,depk,1,1,pRgnGrow); 
	// 11
	if (rowi>0 && depk>0 && flagImg[depk-1][colj][rowi-1]==1)
		rgnfindsub(rowi-1,colj,depk-1,1,1,pRgnGrow); 
	// 12
	if (rowi<ImgWid-1 && depk>0 && flagImg[depk-1][colj][rowi+1]==1)
		rgnfindsub(rowi+1,colj,depk-1,1,1,pRgnGrow); 
	// 13
	if (rowi>0 && depk<ImgDep-1 && flagImg[depk+1][colj][rowi-1]==1)
		rgnfindsub(rowi-1,colj,depk+1,1,1,pRgnGrow); 
	// 14
	if (rowi<ImgWid-1 && depk<ImgDep-1 && flagImg[depk+1][colj][rowi+1]==1)
		rgnfindsub(rowi+1,colj,depk+1,1,1,pRgnGrow); 
	// 15
	if (colj>0 && depk>0 && flagImg[depk-1][colj-1][rowi]==1)
		rgnfindsub(rowi,colj-1,depk-1,1,1,pRgnGrow); 
	// 16
	if (colj<ImgHei-1 && depk>0 && flagImg[depk-1][colj+1][rowi]==1)
		rgnfindsub(rowi,colj+1,depk-1,1,1,pRgnGrow); 
	// 17
	if (colj>0 && depk<ImgDep-1 && flagImg[depk+1][colj-1][rowi]==1)
		rgnfindsub(rowi,colj-1,depk+1,1,1,pRgnGrow); 
	// 18
	if (colj<ImgHei-1 && depk<ImgDep-1 && flagImg[depk+1][colj+1][rowi]==1)
		rgnfindsub(rowi,colj+1,depk+1,1,1,pRgnGrow);
	// 19
	if (rowi>0 && colj>0 && depk>0 && flagImg[depk-1][colj-1][rowi-1]==1)
		rgnfindsub(rowi-1,colj-1,depk-1,1,1,pRgnGrow);
	// 20
	if (rowi<ImgWid-1 && colj>0 && depk>0 && flagImg[depk-1][colj-1][rowi+1]==1)
		rgnfindsub(rowi+1,colj-1,depk-1,1,1,pRgnGrow);
	// 21
	if (rowi>0 && colj<ImgHei-1 && depk>0 && flagImg[depk-1][colj+1][rowi-1]==1)
		rgnfindsub(rowi-1,colj+1,depk-1,1,1,pRgnGrow);
	// 22
	if (rowi>0 && colj>0 && depk<ImgDep-1 && flagImg[depk+1][colj-1][rowi-1]==1)
		rgnfindsub(rowi-1,colj-1,depk+1,1,1,pRgnGrow);
	// 23
	if (rowi<ImgWid-1 && colj<ImgHei-1 && depk>0 && flagImg[depk-1][colj+1][rowi+1]==1)
		rgnfindsub(rowi+1,colj+1,depk-1,1,1,pRgnGrow);
	// 24
	if (rowi<ImgWid-1 && colj>0 && depk<ImgDep-1 && flagImg[depk+1][colj-1][rowi+1]==1)
		rgnfindsub(rowi+1,colj-1,depk+1,1,1,pRgnGrow);
	// 25
	if (rowi>0 && colj<ImgHei-1 && depk<ImgDep-1 && flagImg[depk+1][colj+1][rowi-1]==1)
		rgnfindsub(rowi-1,colj+1,depk+1,1,1,pRgnGrow);
	// 26
	if (rowi<ImgWid-1 && colj<ImgHei-1 && depk<ImgDep-1 && flagImg[depk+1][colj+1][rowi+1]==1)
		rgnfindsub(rowi+1,colj+1,depk+1,1,1,pRgnGrow);

	
	return;
}



bool RegionGrowing::doRegionGrow(V3DPluginCallback* callback, QWidget *parent)
{
	RegiongrowDialog dialog(callback, parent);
	if (!dialog.image)
		return false;
	
	if (dialog.exec()!=QDialog::Accepted)
		return false;
	
	dialog.update();
	
	Image4DSimple* subject = dialog.image;
	if (!subject)
		return false;
	ROIList pRoiList = dialog.pRoiList;
	
	V3DLONG ch_rgb = dialog.ch;
	V3DLONG th_idx = dialog.th_idx;
	double threshold = dialog.thresh;

	V3DLONG volsz = dialog.volsz;
	
	//
	int start_t = clock(); // record time point
	
	ImagePixelType datatype_subject = subject->getDatatype();
	
	//if(datatype_subject != V3D_UINT8)
	//{
	//	QMessageBox::information(parent, "Information", QString("Currently this program only support 8-bit data."));
	//	return;
	//}
		
	V3DLONG sz0 = subject->getXDim();
    V3DLONG sz1 = subject->getYDim();
    V3DLONG sz2 = subject->getZDim();
	V3DLONG sz3 = subject->getCDim();
	
	V3DLONG pagesz_sub = sz0*sz1*sz2;
	V3DLONG offset_sub = ch_rgb*pagesz_sub;
	
	unsigned char *pSub = 0;
	if(datatype_subject == V3D_UINT8)
	{
		pSub = (unsigned char*)subject->getRawData();
	}
	//-------------------------------------------------------------------------------->>>>>
	// convert to 8-bit
	else
	{
		v3d_msg("Convert float data to unsigned char for the region growing!", 0);
		try
		{
			// original data is float
			float* pImOrig = (float*) subject->getRawData();

			V3DLONG channelsz = sz0*sz1*sz2;
			pSub = new unsigned char [channelsz*sz3];
			for(V3DLONG c=0; c<sz3; c++)
			{
				V3DLONG offset = c*channelsz;
				float maxv = pImOrig[offset], minv = pImOrig[offset];

				for(V3DLONG i=1+offset; i<channelsz+offset; i++)
				{
					float vv = pImOrig[i];
					if(vv > maxv) 
						maxv = vv;
					else if(vv < minv)
						minv = vv;
				}
				maxv -= minv;

				if(fabs(maxv)<0.000001)
				{
					for(V3DLONG j=offset; j<channelsz+offset; j++)
						pSub[j] = (unsigned char)0;
				}
				else
				{
					double sf = 255.0/maxv;
					for(V3DLONG j=offset; j<channelsz+offset; j++)
						pSub[j] = (unsigned char)(sf*(pImOrig[j]-minv));
				}
			}
		}
		catch(...)
		{
			v3d_msg("Error allocating memory for image.\n");
			return false;
		}
	}
	//--------------------------------------------------------------------------------<<<<<<<<<<<

	//finding the bounding box of ROI
	bool vxy=true,vyz=true,vzx=true; // 3 2d-views
	
	QRect b_xy = pRoiList.at(0).boundingRect();
	QRect b_yz = pRoiList.at(1).boundingRect();
	QRect b_zx = pRoiList.at(2).boundingRect();
	
	if(b_xy.left()==-1 || b_xy.top()==-1 || b_xy.right()==-1 || b_xy.bottom()==-1)
		vxy=false;
	if(b_yz.left()==-1 || b_yz.top()==-1 || b_yz.right()==-1 || b_yz.bottom()==-1)
		vyz=false;
	if(b_zx.left()==-1 || b_zx.top()==-1 || b_zx.right()==-1 || b_zx.bottom()==-1)
		vzx=false;
		
	V3DLONG bpos_x, bpos_y, bpos_z, bpos_c, epos_x, epos_y, epos_z, epos_c;
	
	// 8 cases
	if(vxy && vyz && vzx) // all 3 2d-views
	{
		bpos_x = qBound(V3DLONG(0), V3DLONG(qMax(b_xy.left(), b_zx.left())), sz0-1);
		bpos_y = qBound(V3DLONG(0), V3DLONG(qMax(b_xy.top(),  b_yz.top())), sz1-1);
		bpos_z = qBound(V3DLONG(0), V3DLONG(qMax(b_yz.left(), b_zx.top())), sz2-1);
		
		epos_x = qBound(V3DLONG(0), V3DLONG(qMin(b_xy.right(), b_zx.right())), sz0-1);
		epos_y = qBound(V3DLONG(0), V3DLONG(qMin(b_xy.bottom(), b_yz.bottom())), sz1-1);
		epos_z = qBound(V3DLONG(0), V3DLONG(qMin(b_yz.right(), b_zx.bottom())), sz2-1);
	}
	else if(!vxy && vyz && vzx) // 2 of 3
	{
		bpos_x = qBound(V3DLONG(0), V3DLONG(qMax(0, b_zx.left())), sz0-1);
		bpos_y = qBound(V3DLONG(0), V3DLONG(qMax(0,  b_yz.top())), sz1-1);
		bpos_z = qBound(V3DLONG(0), V3DLONG(qMax(b_yz.left(), b_zx.top())), sz2-1);
		
		epos_x = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)(sz0-1), (V3DLONG)b_zx.right())), sz0-1);
		epos_y = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)(sz1-1), (V3DLONG)b_yz.bottom())), sz1-1);
		epos_z = qBound(V3DLONG(0), V3DLONG(qMin(b_yz.right(), b_zx.bottom())), sz2-1);
	}
	else if(vxy && !vyz && vzx)
	{
		bpos_x = qBound(V3DLONG(0), V3DLONG(qMax(b_xy.left(), b_zx.left())), sz0-1);
		bpos_y = qBound(V3DLONG(0), V3DLONG(qMax(b_xy.top(),  0)), sz1-1);
		bpos_z = qBound(V3DLONG(0), V3DLONG(qMax(0, b_zx.top())), sz2-1);
		
		epos_x = qBound(V3DLONG(0), V3DLONG(qMin(b_xy.right(), b_zx.right())), sz0-1);
		epos_y = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)b_xy.bottom(), (V3DLONG)(sz1-1))), sz1-1);
		epos_z = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)(sz2-1), (V3DLONG)b_zx.bottom())), sz2-1);
	}
	else if(vxy && vyz && !vzx)
	{
		bpos_x = qBound(V3DLONG(0), V3DLONG(qMax(b_xy.left(), 0)), sz0-1);
		bpos_y = qBound(V3DLONG(0), V3DLONG(qMax(b_xy.top(),  b_yz.top())), sz1-1);
		bpos_z = qBound(V3DLONG(0), V3DLONG(qMax(b_yz.left(), 0)), sz2-1);
		
		epos_x = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)b_xy.right(), (V3DLONG)(sz0-1))), sz0-1);
		epos_y = qBound(V3DLONG(0), V3DLONG(qMin(b_xy.bottom(), b_yz.bottom())), sz1-1);
		epos_z = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)b_yz.right(), (V3DLONG)(sz2-1))), sz2-1);
	}
	else if(vxy && !vyz && !vzx) // only 1 of 3
	{
		bpos_x = qBound(V3DLONG(0), V3DLONG(qMax(b_xy.left(), 0)), sz0-1);
		bpos_y = qBound(V3DLONG(0), V3DLONG(qMax(b_xy.top(),  0)), sz1-1);
		bpos_z = 0;
		
		epos_x = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)b_xy.right(), (V3DLONG)(sz0-1))), sz0-1);
		epos_y = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)b_xy.bottom(), (V3DLONG)(sz1-1))), sz1-1);
		epos_z = sz2-1;
	}
	else if(!vxy && vyz && !vzx)
	{
		bpos_x = 0;
		bpos_y = qBound(V3DLONG(0), V3DLONG(qMax(0,  b_yz.top())), sz1-1);
		bpos_z = qBound(V3DLONG(0), V3DLONG(qMax(b_yz.left(), 0)), sz2-1);
		
		epos_x = sz0-1;
		epos_y = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)(sz1-1), (V3DLONG)b_yz.bottom())), sz1-1);
		epos_z = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)b_yz.right(), (V3DLONG)(sz2-1))), sz2-1);
	}
	else if(!vxy && !vyz && vzx)
	{
		bpos_x = qBound(V3DLONG(0), V3DLONG(qMax(0, b_zx.left())), sz0-1);
		bpos_y = 0;
		bpos_z = qBound(V3DLONG(0), V3DLONG(qMax(0, b_zx.top())), sz2-1);
		
		epos_x = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)(sz0-1), (V3DLONG)b_zx.right())), sz0-1);
		epos_y = sz1-1;
		epos_z = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)(sz2-1), (V3DLONG)b_zx.bottom())), sz2-1);
	}
	else // 0
	{
		bpos_x = 0;
		bpos_y = 0;
		bpos_z = 0;
		
		epos_x = sz0-1;
		epos_y = sz1-1;
		epos_z = sz2-1;
	}

	//qDebug("x %d y %d z %d x %d y %d z %d ",bpos_x,bpos_y,bpos_z,epos_x,epos_y,epos_z);

	//ROI extraction
	V3DLONG sx = (epos_x-bpos_x)+1;
    V3DLONG sy = (epos_y-bpos_y)+1;
    V3DLONG sz = (epos_z-bpos_z)+1;
	V3DLONG sc = sz3; // 0,1,2
	
	//choose the channel stack
	V3DLONG pagesz = sx*sy*sz;
	
	double meanv=0, stdv=0;

	//------------------------------------------------------------------------------------------------------------------------------------	
	unsigned char *data1d = NULL;
	
	try
	{
		data1d = new unsigned char [pagesz];

		unsigned char *pSubject = pSub + offset_sub; 
		
		for(V3DLONG k=bpos_z; k<=epos_z; k++)
		{
			V3DLONG offset_z = k*sz0*sz1;
			V3DLONG offset_crop_z = (k-bpos_z)*sx*sy;
			for(V3DLONG j=bpos_y; j<=epos_y; j++)
			{				
				V3DLONG offset_y = j*sz0 + offset_z;
				V3DLONG offset_crop_y = (j-bpos_y)*sx + offset_crop_z;
				for(V3DLONG i=bpos_x; i<=epos_x; i++)
				{
					data1d[(i-bpos_x) + offset_crop_y] = pSubject[i+offset_y];			
					meanv += data1d[(i-bpos_x) + offset_crop_y];
				}
			}
		}
	}
	catch(...)
	{
		printf("Fail to allocate memory.\n");
		return false;
	}
	
	meanv /= pagesz;
	
	for(V3DLONG i=0; i<pagesz; i++)
		stdv += ((double)data1d[i] - meanv)*((double)data1d[i] - meanv);
	
	stdv /= (pagesz-1);
	stdv = sqrt(stdv);
	
	qDebug() << "meanv ..." << meanv << "stdv ..." << stdv;

	//----------------------------------------------------------------------------------------------------------------------------------
	// bw
	unsigned char *bw = new unsigned char [pagesz];
	
	if(th_idx == 0)
	{
		threshold = meanv;
	}
	else if(th_idx == 1)
	{
		threshold = meanv + stdv;
	}	
	
	for(V3DLONG i=0; i<pagesz; i++)
		bw[i] = ((double)data1d[i]>threshold)?1:0;
	
	
	//
	V3DLONG offset_y, offset_z;
	
	offset_y=sx;
	offset_z=sx*sy;
	
    V3DLONG neighbors = 26;
	V3DLONG neighborhood_26[26] = {-1, 1, -offset_y, offset_y, -offset_z, offset_z,
		-offset_y-1, -offset_y+1, -offset_y-offset_z, -offset_y+offset_z, 
		offset_y-1, offset_y+1, offset_y-offset_z, offset_y+offset_z,
		offset_z-1, offset_z+1, -offset_z-1, -offset_z+1,
		-1-offset_y-offset_z, -1-offset_y+offset_z, -1+offset_y-offset_z, -1+offset_y+offset_z,
        1-offset_y-offset_z, 1-offset_y+offset_z, 1+offset_y-offset_z, 1+offset_y+offset_z}; 
	
	// eliminate volume with only one single voxel
	for(V3DLONG k = 0; k < sz; k++) 
	{				
		V3DLONG idxk = k*offset_z;
		for(V3DLONG j = 0;  j < sy; j++) 
		{
			V3DLONG idxj = idxk + j*offset_y;
			for(V3DLONG i = 0, idx = idxj; i < sx;  i++, idx++) 
			{
				if(i==0 || i==sx-1 || j==0 || j==sy-1 || k==0 || k==sz-1)
					continue;
				
				if(bw[idx])
				{
					bool one_point = true;
					for(int ineighbor=0; ineighbor<neighbors; ineighbor++)
					{
						V3DLONG n_idx = idx + neighborhood_26[ineighbor];
						
						if(bw[n_idx])
						{
							one_point = false;
							break;
						}
						
					}
					if(one_point==true)
						bw[idx] = 0;
				}
				
			}
		}
	}
	
	// display BW image
	//Image4DSimple p4Dbw;
	//p4Dbw.setData((unsigned char*)bw, sx, sy, sz, 1, V3D_UINT8);
	//
	//v3dhandle newwinbw = callback.newImageWindow();
	//callback.setImage(newwinbw, &p4Dbw);
	//callback.setImageName(newwinbw, "bw");
	//callback.updateImageWindow(newwinbw);
	
	// 3D region growing
	//----------------------------------------------------------------------------------------------------------------------------------
	
	//declaration
	V3DLONG totalpxlnum=pagesz;
	
	RgnGrow3dClass * pRgnGrow = new RgnGrow3dClass();
	if (!pRgnGrow)
	{
		printf("Fail to allocate memory for RgnGrow3dClass().");
	}
	
	pRgnGrow->ImgDep = sz;
	pRgnGrow->ImgHei = sy;
	pRgnGrow->ImgWid = sx;
	
	newIntImage3dPairMatlabProtocal(pRgnGrow->quantImg3d,pRgnGrow->quantImg1d,pRgnGrow->ImgDep,pRgnGrow->ImgHei,pRgnGrow->ImgWid);
	
	int nstate;
	UBYTE minlevel,maxlevel;
	
	copyvecdata((unsigned char *)bw,totalpxlnum,pRgnGrow->quantImg1d,nstate,minlevel,maxlevel); 
	
	//don't count lowest level (background here = 0)
	minlevel = minlevel+1;
	if (minlevel>maxlevel) 
		minlevel = maxlevel; //at least do one level
	
	//begin computation
	phcDebugPosNum = 0;
	phcDebugRgnNum = 0;
	
	newIntImage3dPairMatlabProtocal(pRgnGrow->PHCLABELSTACK3d,pRgnGrow->PHCLABELSTACK1d,1,3,totalpxlnum);
	pRgnGrow->PHCLABELSTACKPOS = 0;
	
	pRgnGrow->PHCURGN = new RGN;
	if (!pRgnGrow->PHCURGN)
	{
		printf("Unable to do:pRgnGrow->PHCURGN = new RGN;  -->current phcDebugRgnNum=%i.\n",phcDebugRgnNum);
	}
	pRgnGrow->PHCURGN_head = pRgnGrow->PHCURGN;
	pRgnGrow->TOTALRGNnum = 1;
	
	deleteIntImage3dPairMatlabProtocal(pRgnGrow->PHCDONEIMG3d,pRgnGrow->PHCDONEIMG1d);
	newIntImage3dPairMatlabProtocal(pRgnGrow->PHCDONEIMG3d,pRgnGrow->PHCDONEIMG1d,pRgnGrow->ImgDep,pRgnGrow->ImgHei,pRgnGrow->ImgWid);
	
	for(int j=minlevel;j<=maxlevel;j++)
	{
		int depk, colj, rowi;
		
		BYTE * PHCDONEIMG1d = pRgnGrow->PHCDONEIMG1d;
		UBYTE * quantImg1d = pRgnGrow->quantImg1d;
		BYTE *** flagImg = pRgnGrow->PHCDONEIMG3d;
		for (V3DLONG tmpi=0; tmpi<totalpxlnum; tmpi++)
		{
			PHCDONEIMG1d[tmpi] = (quantImg1d[tmpi]==(UBYTE)j)?1:0;
		}
		
		pRgnGrow->PHCURLABEL = 0;
		
		for(depk=0; depk<pRgnGrow->ImgDep; depk++)
			for(colj=0; colj<pRgnGrow->ImgHei; colj++)
				for(rowi=0; rowi<pRgnGrow->ImgWid; rowi++)
				{
					if (flagImg[depk][colj][rowi]==1)
					{
						pRgnGrow->IFINCREASELABEL = 1;
						
						pRgnGrow->PHCURLABEL++;
						
						pRgnGrow->PHCLABELSTACKPOS = 0;
						
						pRgnGrow->PHCLABELSTACK3d[0][0][pRgnGrow->PHCLABELSTACKPOS] = depk;
						pRgnGrow->PHCLABELSTACK3d[0][1][pRgnGrow->PHCLABELSTACKPOS] = colj;
						pRgnGrow->PHCLABELSTACK3d[0][2][pRgnGrow->PHCLABELSTACKPOS] = rowi;
						
						//create pos memory
						pRgnGrow->PHCURGNPOS = new POS;
						if (pRgnGrow->PHCURGNPOS==0)
						{
							printf("Fail to allocate memory! -->cur phcDebugPosNum=%i.\n",phcDebugPosNum);
						}
						pRgnGrow->PHCURGNPOS_head = pRgnGrow->PHCURGNPOS;
						pRgnGrow->TOTALPOSnum = 1;
						
						while(1)
						{
							pRgnGrow->IFINCREASELABEL = 1;
							V3DLONG posbeg = pRgnGrow->PHCLABELSTACKPOS;
							V3DLONG mypos = posbeg;
							
							while (mypos>=0)
							{
								pRgnGrow->STACKCNT = 0;
								int curdep = pRgnGrow->PHCLABELSTACK3d[0][0][mypos];
								int curcol = pRgnGrow->PHCLABELSTACK3d[0][1][mypos];
								int currow = pRgnGrow->PHCLABELSTACK3d[0][2][mypos];
								
								if (flagImg[curdep][curcol][currow]==1)
								{
									rgnfindsub(currow,curcol,curdep,0,1,pRgnGrow);
								}
								else if(flagImg[curdep][curcol][currow]==-1)
								{
									rgnfindsub(currow,curcol,curdep,0,0,pRgnGrow);
								}
								
								V3DLONG posend = pRgnGrow->PHCLABELSTACKPOS;
								
								if (posend>posbeg) 
								{mypos = pRgnGrow->PHCLABELSTACKPOS;}
								else 
								{mypos = mypos-1;}
								posbeg = posend;
							}
							
							if (pRgnGrow->IFINCREASELABEL==1)
								break;
						}
						
						//set pos as member of current RGN
						pRgnGrow->PHCURGN->layer = j;
						pRgnGrow->PHCURGN->no = pRgnGrow->PHCURLABEL;
						pRgnGrow->PHCURGN->poslist = pRgnGrow->PHCURGNPOS_head;
						pRgnGrow->PHCURGN->poslistlen = pRgnGrow->TOTALPOSnum;
						
						pRgnGrow->TOTALPOSnum = 0;
						
						pRgnGrow->PHCURGN->next = new RGN;
						if(pRgnGrow->PHCURGN->next==0)
						{printf("fail to do --> pRgnGrow->PHCURGN->next = new RGN;\n");}
						pRgnGrow->PHCURGN = pRgnGrow->PHCURGN->next;
						pRgnGrow->TOTALRGNnum++;
					}
				} 
	}
	//de-alloc
	if(data1d) {delete []data1d; data1d = NULL;}
	
	//
	int end_t = clock();
	qDebug() << "label objects via region growing time elapse ..." << end_t-start_t;

	//find the second big area in labeling
	STCL *staRegion = new STCL;
	STCL *staRegion_begin = staRegion;
	RGN *curRgn = pRgnGrow->PHCURGN_head;
	V3DLONG nrgncopied = 0; //num of rgn output
	
	std::vector<STCL> stclList;
	
	while(curRgn && curRgn->next)
	{
		qDebug() << "num of rgn ..." << nrgncopied << curRgn->no << curRgn->next->no;
		
		staRegion->no = curRgn->no;
		staRegion->count = 0;
		
		POS * curPos = curRgn->poslist;
		
		V3DLONG count = 0;
		staRegion->desposlist = new V3DLONG [curRgn->poslistlen-1];
		while(curPos && curPos->next)
		{
			staRegion->desposlist[count++] = curPos->pos;
			curPos = curPos->next;
		}
		staRegion->count = count;
		
		qDebug() << "pixels ..." << count;
		
		if(count<volsz) { nrgncopied++; curRgn = curRgn->next; continue; } // filter out the small components
		
		//
		stclList.push_back(*staRegion);
		
		//
		curRgn = curRgn->next;
		staRegion->next = new STCL;
		staRegion = staRegion->next;
		
		nrgncopied++; 
		
	}
	
	// result pointer
	V3DLONG length;
	
	V3DLONG n_rgn = stclList.size(); // qMin(5, nrgncopied);
	
	qDebug() << "display "<< n_rgn<<" rgns from "<< nrgncopied;
	
	if(n_rgn>65534)
	{
		float *pRGCL = NULL;
		try
		{
			pRGCL = new float [pagesz];
			
			memset(pRGCL, 0, sizeof(float)*pagesz);
		}
		catch (...) 
		{
			printf("Fail to allocate memory.\n");
			return false;
		}
		
		LandmarkList cmList;
		
		for(int ii=0; ii<n_rgn; ii++)
		{
			length = stclList.at(ii).count; //a[ii];
			
			qDebug() << "region ..." << ii << length;
			
			// find idx
			V3DLONG *cutposlist = stclList.at(ii).desposlist;
			
			float scx=0,scy=0,scz=0,si=0;
			
			for(int i=0; i<length; i++)
			{
				//qDebug() << "idx ..." << i << cutposlist[i] << pagesz;
				
				pRGCL[ cutposlist[i] ] = (float)ii + 1.0;
				
				float cv = pSub[ cutposlist[i] ];
				
				V3DLONG idx = cutposlist[i];
				
				V3DLONG k1 = idx/(sx*sy);
				V3DLONG j1 = (idx - k1*sx*sy)/sx;
				V3DLONG i1 = idx - k1*sx*sy - j1*sx;
				
				scz += k1*cv;
				scy += j1*cv;
				scx += i1*cv;
				si += cv;
			}
			
			if (si>0)
			{
				V3DLONG ncx = scx/si + 0.5 +1; 
				V3DLONG ncy = scy/si + 0.5 +1; 
				V3DLONG ncz = scz/si + 0.5 +1;
				
				LocationSimple pp(ncx, ncy, ncz);
				cmList.push_back(pp);
				
			}
			
		}
		
		// get bounding box
		// always use the first slice to get the ROI bounding box in XY plane
		V3DLONG k=0;
		V3DLONG offset_k=k * sx * sy;
		bool bFirst = true;

		for(V3DLONG j=0; j<sy; j++)
		{
			V3DLONG offset = offset_k + j*sx; 
			
			for(V3DLONG i=0; i<sx; i++)
			{
				float tmp = pRGCL[offset+i]; 
				if(tmp > 0.0)
				{
					if(bFirst)
					{
						bbox_minx=bbox_maxx=i;
						bbox_miny=bbox_maxy=j;
						bFirst = false;
					}
					// get border
					if(i<bbox_minx) bbox_minx=i;
					else if(i>bbox_maxx) bbox_maxx=i;

					if(j<bbox_miny) bbox_miny=j;
					else if(j>bbox_maxy) bbox_maxy=j;
				}
			}
		}
		printf("Bounding box in segmented image: (%d, %d)-(%d, %d)\n", bbox_minx, bbox_miny, bbox_maxx, bbox_maxy);
		//---------------------------------

		// display
		Image4DSimple p4DImage;
		p4DImage.setData((unsigned char*)pRGCL, sx, sy, sz, 1, V3D_FLOAT32);
		v3dhandle newwin = callback->newImageWindow();
		callback->setImage(newwin, &p4DImage);
		callback->setImageName(newwin, "region_growing");
		//callback.setLandmark(newwin, cmList); // center of mass
		callback->updateImageWindow(newwin);		
	}
	else if(n_rgn>254)
	{
		unsigned short *pRGCL = NULL;
		try
		{
			pRGCL = new unsigned short [pagesz];
			
			memset(pRGCL, 0, sizeof(unsigned short)*pagesz);
		}
		catch (...) 
		{
			printf("Fail to allocate memory.\n");
			return false;
		}
		
		LandmarkList cmList;
		
		for(int ii=0; ii<n_rgn; ii++)
		{
			length = stclList.at(ii).count; //a[ii];
			
			qDebug() << "region ..." << ii << length;
			
			// find idx
			V3DLONG *cutposlist = stclList.at(ii).desposlist;
			
			float scx=0,scy=0,scz=0,si=0;
			
			for(int i=0; i<length; i++)
			{
				//qDebug() << "idx ..." << i << cutposlist[i] << pagesz;
				
				pRGCL[ cutposlist[i] ] = (unsigned short)ii + 1;
				
				float cv = pSub[ cutposlist[i] ];
				
				V3DLONG idx = cutposlist[i];
				
				V3DLONG k1 = idx/(sx*sy);
				V3DLONG j1 = (idx - k1*sx*sy)/sx;
				V3DLONG i1 = idx - k1*sx*sy - j1*sx;
				
				scz += k1*cv;
				scy += j1*cv;
				scx += i1*cv;
				si += cv;
			}
			
			if (si>0)
			{
				V3DLONG ncx = scx/si + 0.5 +1; 
				V3DLONG ncy = scy/si + 0.5 +1; 
				V3DLONG ncz = scz/si + 0.5 +1;
				
				LocationSimple pp(ncx, ncy, ncz);
				cmList.push_back(pp);
			}			
		}
		
		// get bounding box
		// always use the first slice to get the ROI bounding box in XY plane
		V3DLONG k=0;
		V3DLONG offset_k=k * sx * sy;
		bool bFirst = true;

		for(V3DLONG j=0; j<sy; j++)
		{
			V3DLONG offset = offset_k + j*sx; 
			
			for(V3DLONG i=0; i<sx; i++)
			{
				unsigned short tmp = pRGCL[offset+i]; 
				if(tmp > 0)
				{
					if(bFirst)
					{
						bbox_minx=bbox_maxx=i;
						bbox_miny=bbox_maxy=j;
						bFirst = false;
					}
					// get border			
					if(i<bbox_minx) bbox_minx=i;
					else if(i>bbox_maxx) bbox_maxx=i;

					if(j<bbox_miny) bbox_miny=j;
					else if(j>bbox_maxy) bbox_maxy=j;
				}
			}
		}
		printf("Bounding box in segmented image: (%d, %d)-(%d, %d)\n", bbox_minx, bbox_miny, bbox_maxx, bbox_maxy);
		//--------------------------------

		// display
		Image4DSimple p4DImage;
		p4DImage.setData((unsigned char*)pRGCL, sx, sy, sz, 1, V3D_UINT16);
		
		v3dhandle newwin = callback->newImageWindow();
		callback->setImage(newwin, &p4DImage);
		callback->setImageName(newwin, "region_growing");
		//callback.setLandmark(newwin, cmList); // center of mass
		callback->updateImageWindow(newwin);
	}
	else
	{        
		unsigned char *pRGCL = NULL;
		try
		{
			pRGCL = new unsigned char [pagesz];

			memset(pRGCL, 0, pagesz);
		}
		catch (...) 
		{
			printf("Fail to allocate memory.\n");
			return false;
		}
		
		LandmarkList cmList;
		
		for(int ii=0; ii<n_rgn; ii++)
		{
			length = stclList.at(ii).count; //a[ii];
			
			qDebug() << "region ..." << ii << length;
			
			// find idx
			V3DLONG *cutposlist = stclList.at(ii).desposlist;
			
			float scx=0,scy=0,scz=0,si=0;
			
			for(int i=0; i<length; i++)
			{
				//qDebug() << "idx ..." << i << cutposlist[i] << pagesz;
				
				pRGCL[ cutposlist[i] ] = (unsigned char)ii + 1;
				
				float cv = pSub[ cutposlist[i] ];
				
				V3DLONG idx = cutposlist[i];
				
				V3DLONG k1 = idx/(sx*sy);
				V3DLONG j1 = (idx - k1*sx*sy)/sx;
				V3DLONG i1 = idx - k1*sx*sy - j1*sx;
				
				scz += k1*cv;
				scy += j1*cv;
				scx += i1*cv;
				si += cv;
			}
			
			if (si>0)
			{
				V3DLONG ncx = scx/si + 0.5 +1; 
				V3DLONG ncy = scy/si + 0.5 +1; 
				V3DLONG ncz = scz/si + 0.5 +1;
				
				LocationSimple pp(ncx, ncy, ncz);
				cmList.push_back(pp);	
			}			
		}
		
		// get bounding box
		// always use the first slice to get the ROI bounding box in XY plane
		V3DLONG k=0;
		V3DLONG offset_k=k * sx * sy;
		bool bFirst = true;

		for(V3DLONG j=0; j<sy; j++)
		{
			V3DLONG offset = offset_k + j*sx; 
			for(V3DLONG i=0; i<sx; i++)
			{
				unsigned char tmp = pRGCL[offset+i]; 
				if(tmp > 0)
				{
					if(bFirst)
					{
						bbox_minx=bbox_maxx=i;
						bbox_miny=bbox_maxy=j;
						bFirst = false;
					}
					// get border			
					if(i<bbox_minx) bbox_minx=i;
					else if(i>bbox_maxx) bbox_maxx=i;

					if(j<bbox_miny) bbox_miny=j;
					else if(j>bbox_maxy) bbox_maxy=j;
				}
			}
		}
		printf("Bounding box in segmented image: (%d, %d)-(%d, %d)\n", bbox_minx, bbox_miny, bbox_maxx, bbox_maxy);
		//-------------------------------
		
		// display
		Image4DSimple p4DImage;
		p4DImage.setData((unsigned char*)pRGCL, sx, sy, sz, 1, V3D_UINT8);
		
		v3dhandle newwin = callback->newImageWindow();
		callback->setImage(newwin, &p4DImage);
		callback->setImageName(newwin, "region_growing");
		//callback.setLandmark(newwin, cmList); // center of mass
		callback->updateImageWindow(newwin);
	}

	// only delete pSub created by "new" previously
	if(datatype_subject != V3D_UINT8)
	{
		if (pSub) {delete []pSub; pSub=0;}
	}

	int end_t_t = clock();
	
	qDebug() << "time elapse ..." << end_t_t - end_t;
	
	return true;	
}