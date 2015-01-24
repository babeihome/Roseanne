/*   $Id: DmpEvtBgoShower.cc, 2015-01-15 16:40:51 DAMPE $
 *--------------------------------------------------------
 *  Author(s):
 *
 *--------------------------------------------------------
*/

//#include <time.h>

#include <math.h>
#include <iostream>
#include "TH2D.h"
#include "TF1.h"
#include "DmpEvtBgoShower.h"

ClassImp(DmpBgoFiredBar)
ClassImp(DmpEvtBgoCluster)
ClassImp(DmpEvtBgoShower)

//-------------------------------------------------------------------
DmpBgoFiredBar::DmpBgoFiredBar()
{
  Reset();
}

//-------------------------------------------------------------------
DmpBgoFiredBar::DmpBgoFiredBar(int l,int b,double e,double e0,double e1):fLayer(l),fBar(b),fE(e),fE0(e0),fE1(e1)
{
  if(e0 == 0 || e1 ==0){
    std::cout<<"+++++--> one side readout:\t";
    this->MyPrint();
    //string ask = "";
    //std::cout<<"Continue?   (y/No)";
    //cin>>ask;
    //if(ask == "No"){
    //  throw;
    //}
  }
  fPosition.SetXYZ(0,0,0);
}

//-------------------------------------------------------------------
DmpBgoFiredBar::DmpBgoFiredBar(int l,int b,double e,double e0,double e1,TVector3 v):fLayer(l),fBar(b),fE(e),fE0(e0),fE1(e1),fPosition(v)
{
}

//-------------------------------------------------------------------
DmpBgoFiredBar::~DmpBgoFiredBar()
{
}

//-------------------------------------------------------------------
void DmpBgoFiredBar::Reset()
{
  fLayer = 0;
  fBar = 0;
  fE = 0;
  fE0 = 0;
  fE1 = 0;
  fPosition.SetXYZ(0,0,0);
}

//-------------------------------------------------------------------
void DmpBgoFiredBar::LoadFrom(const DmpBgoFiredBar *&r)
{
  fLayer = r->fLayer;
  fBar = r->fBar;
  fE = r->fE;
  fE0 = r->fE0;
  fE1 = r->fE1;
  fPosition = r->fPosition;
}

void DmpBgoFiredBar::MyPrint()const
{
  std::cout<<"\tl="<<fLayer<<"\tb="<<fBar<<"\tE=("<<fE<<", "<<fE0<<", "<<fE1<<")\tPosi=("<<fPosition.x()<<", "<<fPosition.y()<<", "<<fPosition.z()<<")"<<std::endl;
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
DmpEvtBgoCluster::DmpEvtBgoCluster()
{
  fFiredBar = new TClonesArray("DmpBgoFiredBar",5);
  Reset();
}

//-------------------------------------------------------------------
DmpEvtBgoCluster::DmpEvtBgoCluster(int l,int s,double e):fLayer(l),fSeedBarID(s),fTotE(e)
{
  fFiredBar = new TClonesArray("DmpBgoFiredBar",5);
}

//-------------------------------------------------------------------
DmpEvtBgoCluster::~DmpEvtBgoCluster()
{
  Reset();
  delete fFiredBar;
}

//-------------------------------------------------------------------
void DmpEvtBgoCluster::Reset()
{
  fLayer = 0;
  fSeedBarID = 0;
  fTotE = 0;
  fCenter.SetXYZ(0.,0.,0.);
  if(fFiredBar->GetEntriesFast()){
    fFiredBar->Delete();
  }
}

//-------------------------------------------------------------------
void DmpEvtBgoCluster::LoadFrom(const DmpEvtBgoCluster *&r)
{
  Reset();
  fLayer = r->fLayer;
  fSeedBarID = r->fSeedBarID;
  fTotE = r->fTotE;
  fCenter = r->fCenter;
  for(int i=0;i<r->fFiredBar->GetEntriesFast();++i){
    DmpBgoFiredBar *lef = dynamic_cast<DmpBgoFiredBar*>(fFiredBar->New(i));
    const DmpBgoFiredBar *rig = dynamic_cast<DmpBgoFiredBar*>(r->fFiredBar->At(i));
    lef->LoadFrom(rig);
  }
}

//-------------------------------------------------------------------
int DmpEvtBgoCluster::GetClusterSize()const
{
  return fFiredBar->GetEntriesFast();
}

//-------------------------------------------------------------------
DmpBgoFiredBar *DmpEvtBgoCluster::GetSeedBar()const
{
  DmpBgoFiredBar *seedBar = 0;
  double max_e = 0;
  for(int i=0;i<fFiredBar->GetEntriesFast();++i){
    DmpBgoFiredBar *it = dynamic_cast<DmpBgoFiredBar*>(fFiredBar->At(i));
    if(it->fE > max_e){
      seedBar = it;
      max_e = seedBar->fE;
    }
  }
  if(seedBar){
    seedBar = (seedBar->fBar == fSeedBarID) ? seedBar : 0;
  }
  return seedBar;
}

//-------------------------------------------------------------------
double DmpEvtBgoCluster::GetWindowEnergy(int len)const
{
  double e = 0;
  int n = fFiredBar->GetEntriesFast();
  for(int i=0;i<n;++i){
    DmpBgoFiredBar *aB = dynamic_cast<DmpBgoFiredBar*>(fFiredBar->At(i));
    if((aB->fBar - fSeedBarID)<len){
      e += aB->fE;
    }
  }
  return e;
}

//-------------------------------------------------------------------
double DmpEvtBgoCluster::GetCoGBarID()const
{
  double v=0;
  for(int i=0;i<fFiredBar->GetEntriesFast();++i){
    DmpBgoFiredBar *aB = dynamic_cast<DmpBgoFiredBar*>(fFiredBar->At(i));
    v += aB->fE * (aB->fBar+1);
  }
  v = v / fTotE;
  return (v-1.);
}

void DmpEvtBgoCluster::AddNewFiredBar(DmpBgoFiredBar *ab)
{
  fCenter = fCenter * fTotE + ab->fPosition * ab->fE;
  fTotE += ab->fE;
  fCenter *= (1/fTotE);
  DmpBgoFiredBar *newb = dynamic_cast<DmpBgoFiredBar*>(fFiredBar->New(fFiredBar->GetEntriesFast()));
  const DmpBgoFiredBar *rit = ab;
  newb->LoadFrom(rit);
  //std::cout<<"\tTotE = "<<fTotE<<"\t\tCenter: "<<fCenter.x()<<"\t"<<fCenter.y()<<"\t"<<fCenter.z()<<std::endl;
}

//-------------------------------------------------------------------
void DmpEvtBgoCluster::MyPrint()const
{
  std::cout<<"l="<<fLayer<<"\tseed Bar="<<fSeedBarID<<"\tE="<<fTotE<<"\tPosi=("<<fCenter.x()<<", "<<fCenter.y()<<", "<<fCenter.z()<<std::endl;
  for(int i=0;i<fFiredBar->GetEntriesFast();++i){
    (dynamic_cast<DmpBgoFiredBar*>(fFiredBar->At(i)))->MyPrint();
  }
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
DmpEvtBgoShower::DmpEvtBgoShower()
{
  fClusters = new TClonesArray("DmpEvtBgoCluster",30);
  Reset();
}

//-------------------------------------------------------------------
DmpEvtBgoShower::~DmpEvtBgoShower()
{
  Reset();
  delete fClusters;
}

//-------------------------------------------------------------------
void DmpEvtBgoShower::Reset()
{
  fTotE = 0.;
  fLRMS = 0.;
  for(int i=0;i<BGO_LayerNO;++i){
    fRMS[i] = -1;
    fFValue[i] = -1.;
  }
  for(int i=0;i<5;++i){
    fXZFitPar[i] = 0;
    fYZFitPar[i] = 0;
  }
  if(fClusters->GetEntriesFast()){
    fClusters->Delete();
  }
}

//-------------------------------------------------------------------
void DmpEvtBgoShower::LoadFrom(const DmpEvtBgoShower *&r)
{
  Reset();
  fTotE = r->fTotE;
  fLRMS = r->fLRMS;
  for(int i=0;i<BGO_LayerNO;++i){
    fRMS[i] = r->fRMS[i];
    fFValue[i] = r->fFValue[i];
  }
  for(int i=0;i<5;++i){
    fXZFitPar[i] = r->fXZFitPar[i];
    fYZFitPar[i] = r->fYZFitPar[i];
  }
  for(int i=0;i<r->fClusters->GetEntriesFast();++i){
    DmpEvtBgoCluster *lef = dynamic_cast<DmpEvtBgoCluster*>(fClusters->New(i));
    const DmpEvtBgoCluster *rig = dynamic_cast<DmpEvtBgoCluster*>(r->fClusters->At(i));
    lef->LoadFrom(rig);
  }
}

//-------------------------------------------------------------------
DmpEvtBgoCluster* DmpEvtBgoShower::GetSeedCluster()const
{
  DmpEvtBgoCluster *seedClu =0;
  double max_e = 0;
  for(int i=0;i<fClusters->GetEntriesFast();++i){
    DmpEvtBgoCluster *aC = dynamic_cast<DmpEvtBgoCluster*>(fClusters->At(i));
    if(aC->fTotE > max_e){
      seedClu = aC;
      max_e = aC->fTotE;
    }
  }
  return seedClu;
}

//-------------------------------------------------------------------
double DmpEvtBgoShower::GetTotalEnergy(int layerid)const
{
  double e = 0.;
  for(int i=0;i<fClusters->GetEntriesFast();++i){
    DmpEvtBgoCluster *aC = dynamic_cast<DmpEvtBgoCluster*>(fClusters->At(i));
    if(layerid == aC->fLayer || layerid == -1){
     e += aC->fTotE;
    }
  }
  return e;
}

double DmpEvtBgoShower::GetEnergyOfBar(int l,int barID)const
{
  std::vector<DmpEvtBgoCluster*>  cluInL = this->GetAllClusterInLayer(l);
  for(int i=0;i<cluInL.size();++i){
    for(int b=0;b<cluInL[i]->fFiredBar->GetEntriesFast();++b){
      DmpBgoFiredBar *aB = dynamic_cast<DmpBgoFiredBar*>(cluInL[i]->fFiredBar->At(b));
      if(aB->fBar == barID){
        return aB->fE;
      }
    }
  }
  return -99;
}

//-------------------------------------------------------------------
int DmpEvtBgoShower::GetMaxEnergyLayerID()const
{
  double max_e = 0.;
  int id = -1;
  for(int i=0;i<BGO_LayerNO;++i){
    double ei = this->GetTotalEnergy(i);
    if(ei > max_e){
      max_e = ei;
      id = i;
    }
  }
  return id;
}

//-------------------------------------------------------------------
DmpEvtBgoCluster* DmpEvtBgoShower::GetMaxClusterInLayer(int l)const
{
  DmpEvtBgoCluster *cc = 0;
  double max_e = 0;
  for(int i=0;i<fClusters->GetEntriesFast();++i){
    DmpEvtBgoCluster *aC = dynamic_cast<DmpEvtBgoCluster*>(fClusters->At(i));
    if(aC->fLayer == l){
      if(aC->fTotE > max_e){
        max_e = aC->fTotE;
        cc = aC;
      }
    }
  }
  return cc;
}

//-------------------------------------------------------------------
double DmpEvtBgoShower::GetWindowEnergy(int nBars,int nHalf)const
{
  double TE = 0;
  DmpEvtBgoCluster *aClu = this->GetSeedCluster();
  if(aClu == 0){return TE;}
  int lid = aClu->fLayer;
  TE += aClu->GetWindowEnergy(nBars);
  for(int i=0;i<nHalf;++i){
    aClu = this->GetMaxClusterInLayer(lid-(i+1));   if(aClu){TE += aClu->GetWindowEnergy(nBars);}
    aClu = this->GetMaxClusterInLayer(lid+(i+1));   if(aClu){TE += aClu->GetWindowEnergy(nBars);}
  }
  return TE;
}

int DmpEvtBgoShower::GetFiredBarNumber()const
{
  int b = 0;
  int nClus = fClusters->GetEntriesFast();
  for(int l=0;l<nClus;++l){
    b += (dynamic_cast<DmpEvtBgoCluster*>(fClusters->At(l)))->fFiredBar->GetEntriesFast();
  }
  return b;
}

//-------------------------------------------------------------------
DmpEvtBgoCluster* DmpEvtBgoShower::AddNewCluster(DmpBgoFiredBar *seedBar)
{
  DmpEvtBgoCluster *newC = dynamic_cast<DmpEvtBgoCluster*>(fClusters->New(fClusters->GetEntriesFast()));
  newC->fLayer = seedBar->fLayer;
  newC->fSeedBarID = seedBar->fBar;
  //std::cout<<"===> Add new cluser(layer, seed bar):\t"<<newC->fLayer<<"\t"<<newC->fSeedBarID<<std::endl;
  newC->AddNewFiredBar(seedBar);
  return newC;
}

//-------------------------------------------------------------------
std::vector<DmpEvtBgoCluster*> DmpEvtBgoShower::GetAllClusterInLayer(int l)const
{
  std::vector<DmpEvtBgoCluster*>    cInLayer;
  for(int i=0;i<fClusters->GetEntriesFast();++i){
    DmpEvtBgoCluster *aC = dynamic_cast<DmpEvtBgoCluster*>(fClusters->At(i));
    if(aC->fLayer == l){
      cInLayer.push_back(aC);
    }
  }
  return cInLayer;
}

//-------------------------------------------------------------------
double DmpEvtBgoShower::GetCoGBarIDInLayer(int l)const
{
  double totalEInL = this->GetTotalEnergy(l);
  if(totalEInL == 0){return -1;}

  double cog = 0;
  std::vector<DmpEvtBgoCluster*> cInL = this->GetAllClusterInLayer(l);
  for(int i=0;i<cInL.size();++i){
    cog += (cInL[i]->GetCoGBarID()+1) * cInL[i]->fTotE;
  }
  return (cog/totalEInL - 1);
}

//-------------------------------------------------------------------
Position DmpEvtBgoShower::GetCoGPositionInLayer(int l)const
{
  Position po(0,0,0);
  double totalEInL = this->GetTotalEnergy(l);
  if(totalEInL == 0){return po;}    // po.z() must == 0

  std::vector<DmpEvtBgoCluster*> cInL = this->GetAllClusterInLayer(l);
  for(int i=0;i<cInL.size();++i){
    po += cInL[i]->fCenter * cInL[i]->fTotE;
  }         // p0.z() must != 0, but x(),y() may = 0
  return po;
}

//-------------------------------------------------------------------
double DmpEvtBgoShower::GetPileupRatio()const
{
  return (fClusters->GetEntriesFast() / (double)BGO_LayerNO - 1);
}

#define Z0 58.5         // first layer z
//-------------------------------------------------------------------
Position DmpEvtBgoShower::GetEntryPoint()const
{
  Position p(0,0,Z0);
  p.SetX(fXZFitPar[0]+fXZFitPar[1]*Z0);
  p.SetY(fYZFitPar[0]+fYZFitPar[1]*Z0);
  return p;
}

//-------------------------------------------------------------------
Direction DmpEvtBgoShower::GetTrackDirection()const
{
  Direction d(fXZFitPar[1],fYZFitPar[1],1);
  return d;
}

//-------------------------------------------------------------------
void DmpEvtBgoShower::Calculation()
{
  fTotE = this->GetTotalEnergy();

  fLRMS = 0.;
  std::vector<double>  eInLayer(BGO_LayerNO,0);
{
  for(int i=0;i<BGO_LayerNO;++i){
    eInLayer[i] = (this->GetTotalEnergy(i));
  }
  double cenL = 0;
  for(int i=0;i<BGO_LayerNO;++i){
    cenL += (i+1)*eInLayer[i];
  }
  cenL = cenL/fTotE - 1;
  for(int i=0;i<BGO_LayerNO;++i){
    fLRMS += eInLayer[i] * pow((double)i-cenL,2);
  }
  fLRMS = sqrt(fLRMS / fTotE);
}

  // fRMS, fFValue
  for(int lid=0;lid<BGO_LayerNO;++lid){
    if(eInLayer[lid] == 0) continue;

    double cen = this->GetCoGBarIDInLayer(lid);
    std::vector<DmpBgoFiredBar*>  bInL;   // key
    std::vector<DmpEvtBgoCluster*> cInL = this->GetAllClusterInLayer(lid);
    for(int i=0;i<cInL.size();++i){
      for(int b=0;b<cInL[i]->fFiredBar->GetEntriesFast();++b){
        bInL.push_back(dynamic_cast<DmpBgoFiredBar*>(cInL[i]->fFiredBar->At(b)));
      }
    }
    double rms2_Xt =0;
    for(int b=0;b<bInL.size();++b){
      rms2_Xt += bInL[b]->fE * pow(cen - bInL[b]->fBar,2);
    }
    fFValue[lid] = rms2_Xt / fTotE;
    fRMS[lid] = sqrt(rms2_Xt / eInLayer[lid]);
  }

    //clock_t t3=clock();
    //std::cout<<"\tt3 = DEBUG: "<<__FILE__<<"("<<__LINE__<<")\t\t"<<t3<<std::endl;
    //
  TH2D xz_posi("xz","xz",550,50,600,800,-400,400);
  TH2D yz_posi("yz","yz",550,50,600,800,-400,400);
  static TF1  p1("myf","pol1");
  for(int i=0;i<BGO_LayerNO;++i){
    DmpEvtBgoCluster *aC = this->GetMaxClusterInLayer(i);
    if(aC){
      if(i%2 == 0){
        yz_posi.Fill(aC->fCenter.z(),aC->fCenter.y(),aC->fTotE);
        //yz_posi.SetBinError(aC);
      }else{
        xz_posi.Fill(aC->fCenter.z(),aC->fCenter.x(),aC->fTotE);
      }
    //std::cout<<"\t\tDEBUG i = "<<i<<"\t"<<aC->fCenter.x()<<"\t"<<aC->fCenter.y()<<"\t"<<aC->fCenter.z()<<"\t"<<aC->fTotE<<std::endl;
    }
  }
  if(xz_posi.GetEntries()>1 && yz_posi.GetEntries()>1){
    xz_posi.Fit("myf","QCN");
    fXZFitPar[0] = p1.GetParameter(0);
    fXZFitPar[1] = p1.GetParameter(1);
    fXZFitPar[2] = p1.GetParError(0);
    fXZFitPar[3] = p1.GetParError(1);
    fXZFitPar[4] = p1.GetChisquare();
  //std::cout<<"XZ fit parameters (Pa0,P1,P0_E,P1_E,Chi2):";
  //for(int i=0;i<5;++i){
  //  std::cout<<"\t"<<fXZFitPar[i];
  //}
  //std::cout<<std::endl;
    yz_posi.Fit("myf","QN");
    fYZFitPar[0] = p1.GetParameter(0);
    fYZFitPar[1] = p1.GetParameter(1);
    fYZFitPar[2] = p1.GetParError(0);
    fYZFitPar[3] = p1.GetParError(1);
    fYZFitPar[4] = p1.GetChisquare();
  }
}

//-------------------------------------------------------------------
void DmpEvtBgoShower::MyPrint(bool allInfor)const
{
  std::cout<<"===> Energy, nClusters, Max_E_L, fLRMS:\t"<<fTotE<<"\t"<<fClusters->GetEntriesFast()<<"\t"<<GetMaxEnergyLayerID()<<"\t"<<fLRMS<<std::endl;
  std::cout<<"===> Linear fit parameters (Pa0,P1,P0_E,P1_E,Chi2):\n\tXZ:\t";
  for(int i=0;i<5;++i){
    std::cout<<"\t"<<fXZFitPar[i];
  }
  std::cout<<"\n\tYZ:\t";
  for(int i=0;i<5;++i){
    std::cout<<"\t"<<fYZFitPar[i];
  }
  std::cout<<std::endl;
  std::cout<<"===> RMS and FValue:  X\t\t\tY"<<std::endl;
  for(int i=0;i<BGO_LayerNO/2;++i){
    std::cout<<"\t"<<fRMS[i*2+1]<<", "<<fFValue[i*2+1]<<"\t"<<fRMS[i*2]<<", "<<fFValue[i*2]<<std::endl;
  }
  if(allInfor){
    for(short i=0;i<fClusters->GetEntriesFast();++i){
      (dynamic_cast<DmpEvtBgoCluster*>(fClusters->At(i)))->MyPrint();
    }
  }
}

