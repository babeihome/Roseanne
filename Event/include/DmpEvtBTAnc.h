#ifndef DmpEvtBTAnc_H
#define DmpEvtBTAnc_H

#include "TClonesArray.h" 

class DmpEvtAMSCluster : public TObject{
public:
  //! std constructor (create an empty cluster)
  DmpEvtAMSCluster();
  //! copy constructor 
  DmpEvtAMSCluster(const DmpEvtAMSCluster &orig);
  //! std destructor
  virtual  ~DmpEvtAMSCluster() {};
  //! Fill the hit object with the right infos
  void Build(int lad, int sid, int add, int len, float* sig, float* noi, int* stat, int Sig2NoiRatio, int CNStatus, int PowBits, bool badin=0);
  //! load data from another cluster object
  void LoadFrom(const DmpEvtAMSCluster *&r);
  //! reset the cluster object
  void Reset();
  //! Returns the position of the seed in the Signal vector 
  int   GetSeed()const;
  //! Returns the strip number of the seed
  int   GetSeedAdd()const{return address+GetSeed();}
  //! Returns the signal of the seed strip
  float GetSeedVal()const{return GetCSignal(GetSeed());}
  //! Returns the SN of the seed strip
  float GetSeedSN()const;
  //! Returns the total CLe noise (sq. mean)
  float GetTotNoise()const;
  //! Returns the lenght of the cluster
  int   GetLength()const{return length;}
  //! Returns the lenght of the cluster, cut: SNR > val (default 1.0)
  int   GetLength(float val)const;
  //! Calculate the CoG of the cluster using 2 strips
  float GetCoG()const;
  //! Returns the Total Signal of the cluster
  float GetTotSig()const;
  //! Return the Cluster SN
  float GetTotSN()const{return GetTotSig()/GetTotNoise();}
  //! Caclulate the eta (0.,1.)
  float GetEta()const;
  //! Calculate Etaraw (-0.5,0.5)
  float GetEtaRaw()const;
  //! Calculate center
  float GetCenterPosition()const;
  //! printout the cluster infos
  void Print()const;
  int GoldRegion()const;

public:
  /*
   *    ladder, side, address, snratio, CNstatus, powbits  ---> short
   *
   */
  //! Ladder (0-1 upstream, 2-5 downstream)
  int ladder;
  //! side (0=S, 1=K)
  int side; //0 S / 1 K
  //! Adress of the first strip of the cluster
  int address;
  //! Nstrips of the clusters 
  int length;
  //! Vector of the cluster signals
  std::vector<float> Signal;
  //! Vector of the noise of the cluster strips
  std::vector<float> Noise;
  //! Vector of the status of the cluster strips
  std::vector<int>  Status;
  //! cluster > MAXLENGHT strips
  bool bad;
  //! Golden Flag
  //int golden;
  // Signal/Noise ratio. from binary data
  int snratio;
  // CN status
  int CNstatus;
  // Power bits
  int powbits;

private:
  float GetCSignal(int aa)const;

public:
  ClassDef(DmpEvtAMSCluster,1)
};

//-------------------------------------------------------------------
class DmpEvtBTAnc : public TObject{
public:
  DmpEvtBTAnc();
  virtual ~DmpEvtBTAnc();
  void LoadFrom(const DmpEvtBTAnc *&r);
  void Reset();

public:
  TClonesArray  *fAMSClusters;      // AMS data
  int           fAdcCherenkov1;     // cherenkov 1
  int           fAdcCherenkov2;     // cherenkov 2
  int           fAdcPbGlass;        // behind S3. Beam off
  int           fAdcSc1;    // scintillator behind dump, PMT 1
  int           fAdcSc2;    // scintillator behind dump, PMT 2
  int           fAdcSc3;    // scintillator behind dump, PMT 3
  int           fAdcSc4;    // scintillator behind dump, PMT 4
  int           fAdcSd1;    // scintillator behind DAMPE, PMT 1
  int           fAdcSd2;    // scintillator behind DAMPE, PMT 2

public:
  ClassDef(DmpEvtBTAnc,1)
};

#endif


