/*
 *  $Id: DmpEvtBgoShower.h, 2015-01-21 19:52:06 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 18/01/2015
*/

#ifndef DMPEVT_BgoShower_H
#define DMPEVT_BgoShower_H

#include <vector>
#include "TVector3.h"
#include "TClonesArray.h"

#define BGO_LayerNO 14
#define BGO_BarNO 22

typedef TVector3 Position;
typedef TVector3 Direction;

class DmpBgoFiredBar : public TObject
{
public:
  DmpBgoFiredBar();
  DmpBgoFiredBar(int l,int b,double e=0,double e0=0,double e1=0);
  DmpBgoFiredBar(int l,int b,double e,double e0,double e1,TVector3 v);
  ~DmpBgoFiredBar();
  void Reset();
  void LoadFrom(const DmpBgoFiredBar *&r);
  void MyPrint()const;

  int       fLayer; // 0~13
  int       fBar;   // 0~21
  double    fE;    // unit: MeV. combined E
  double    fE0;   // side 0
  double    fE1;   // side 1
  Position  fPosition;   // unit: mm

  ClassDef(DmpBgoFiredBar,1);
};

class DmpEvtBgoCluster : public TObject
{
public:
  DmpEvtBgoCluster();
  DmpEvtBgoCluster(int l, int seedBarID,double totalE=0);
  ~DmpEvtBgoCluster();

  void Reset();
  void LoadFrom(const DmpEvtBgoCluster *&r);
  int GetClusterSize()const;   // how many fired bar
  DmpBgoFiredBar *GetSeedBar()const;
  double GetWindowEnergy(int n=3)const;  // n bars around seed bar
  double GetCoGBarID()const;

  void AddNewFiredBar(DmpBgoFiredBar *aBar);
  void MyPrint()const;

public:
  int       fLayer;     // layer id, from 0~13
  int       fSeedBarID; // bar ID of max DmpBgoFiredBar::fE
  double    fTotE;      // total energy. MeV
  Position  fCenter;    // cluster center. unit: mm. NOTE:  one dimension is wrong: layer 0,2,4... y is right, x is wrong
  TClonesArray *fFiredBar;  // ordered by DmpFiredBar::fE, from max to min

  ClassDef(DmpEvtBgoCluster,1)
};

class DmpEvtBgoShower : public TObject
{
public:
  DmpEvtBgoShower();
  ~DmpEvtBgoShower();

  void Reset();
  void LoadFrom(const DmpEvtBgoShower *&r);
  void MyPrint(bool allInformation = true)const;

  DmpEvtBgoCluster* GetSeedCluster()const;
  Position GetEntryPoint()const;
  Direction GetTrackDirection()const;
  double GetTotalEnergy(int layerID = -1)const;
  int  GetMaxEnergyLayerID()const;
  double GetTotalRMS()const;
  double GetPileupRatio()const;     // (-1,0): some layer not fired. 0: one track. > 1: multi-track
  int GetFiredBarNumber()const;

  double GetEnergyOfBar(int layerID, int barID)const;
  DmpEvtBgoCluster *GetMaxClusterInLayer(int layerID)const;
  double GetCoGBarIDInLayer(int layerID)const;
  Position GetCoGPositionInLayer(int layerID)const;
  double GetNormalizedRMS(int layerID)const;
  double GetRFRatio(int layerID)const;  // NOTE:   fRMS[layerID] / fFValue[layerID].consist of information of total energy, energy in layerID and RMS2 in this layer

  double GetEnergyOfEMaxLayer()const;
  double GetEnergyRatioOfEMaxLayer()const;
  double GetWindowEnergy(int nBars=3,int nHalf=1)const;  //nHalf: include how may layers above(below) seed layer. Totally: 2*nHalf + 1 layers
  double GetWindowEnergyRatio(int nBars=3,int nHalf = 1)const;
  double GetRMSOfEMaxLayer()const;
  double GetRFRatioOfEMaxLayer()const;
  int GetLayerIDOfMaxRFRatio()const;
  int GetLayerIDOfMaxRMS()const;
  double GetMaxRFRatio()const;
  double GetMyValue(int nHalfLayer = 2)const;

  std::vector<DmpEvtBgoCluster*> GetAllClusterInLayer(int layerID)const;

public:
  DmpEvtBgoCluster* AddNewCluster(DmpBgoFiredBar *seedBar);
  void Calculation();   // fTotE, Direction fit parameters

public:
  double        fTotE;          // total energy. MeV
  double        fLRMS;          // Longitudinal RMS, by using total energy in each layer, and layer ID along Z axis
  double        fRMS[BGO_LayerNO];  // for each layer. if == -1: no fired bar in this layer. if == 0: only fired one bar
  double        fFValue[BGO_LayerNO]; // for each layer. if == -1: no fired bar in this layer. if == 0: only fired one bar
  double        fXZFitPar[5];      // linear fit parameter of x_z. [0,1] value of parameter 0 and 1. [2,3] error of parameter 0 and 1. [4]: Chi2
  double        fYZFitPar[5];      // linear fit parameter of y_z
  TClonesArray  *fClusters;

  ClassDef(DmpEvtBgoShower,1)
};

#endif
