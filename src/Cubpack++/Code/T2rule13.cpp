/////////////////////////////////////////////////////////
//                                                     //
//    Cubpack++                                        //
//                                                     //
//        A Package For Automatic Cubature             //
//                                                     //
//        Authors : Ronald Cools                       //
//                  Dirk Laurie                        //
//                  Luc Pluym                          //
//                  Bart Maerten                       //  
//                                                     //
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
//File T2rule13.c
//Authors: Berntsen & Espelid (DRLTRI)
//C++Translation: Luc Pluym/Ronald Cools
// History:
//   (date)          (version)
//   19 Aug 1994     V0.1 (first limited distribution)
//   28 Mar 1996     V0.1h(redundant variable removed)
//    1 Aug 1996     V1.0 (paper accepted by ACM TOMS)
//    1 May 1999     V1.2 ( 2/4 division introduced  )
//
/////////////////////////////////////////////////////////

#include "math.h"
#include "T2rule13.h"
#include "tools.h"
#define sqr(x) ((x)*(x))

// coordinates of the quadrature rule

static real G1[] = { 0.333333333333333333333333333333e0,
          0.950275662924105565450352089520e0,
          0.171614914923835347556304795551e0,
          0.539412243677190440263092985511e0,
          0.772160036676532561750285570113e0,
          0.009085399949835353883572964740e0,
          0.062277290305886993497083640527e0,
          0.022076289653624405142446876931e0,
          0.018620522802520968955913511549e0,
          0.096506481292159228736516560903e0,
          0.851306504174348550389457672223e0,
          0.689441970728591295496647976487e0,
          0.635867859433872768286976979827e0};

static real G2[] = { 0.333333333333333333333333333333e0,
          0.024862168537947217274823955239e0,
          0.414192542538082326221847602214e0,
          0.230293878161404779868453507244e0,
          0.113919981661733719124857214943e0,
          0.495457300025082323058213517632e0,
          0.468861354847056503251458179727e0,
          0.851306504174348550389457672223e0,
          0.689441970728591295496647976487e0,
          0.635867859433872768286976979827e0,
          0.022076289653624405142446876931e0,
          0.018620522802520968955913511549e0,
          0.096506481292159228736516560903e0};

//
//   Weights of the degree 13 quadrature rule.
//

static real W[] = {0.051739766065744133555179145422e0,
          0.008007799555564801597804123460e0,
          0.046868898981821644823226732071e0,
          0.046590940183976487960361770070e0,
          0.031016943313796381407646220131e0,
          0.010791612736631273623178240136e0,
          0.032195534242431618819414482205e0,
          0.015445834210701583817692900053e0,
          0.017822989923178661888748319485e0,
          0.037038683681384627918546472190e0,
          0.015445834210701583817692900053e0,
          0.017822989923178661888748319485e0,
          0.037038683681384627918546472190e0};

//   Weights of the first null rule of degree 7.

static real W71[] = {-0.077738051051462052051304462750e0,
          0.001640389740236881582083124927e0,
          0.078124083459915167386776552733e0,
          -0.030706528522391137165581298102e0,
          0.010246307817678312345028512621e0,
          0.012586300774453821540476193059e0,
          -0.043630506151410607808929481439e0,
          -0.004567055157220063810223671248e0,
          0.003393373439889186878847613140e0,
          0.000000000000000000000000000000e0,
          -0.004567055157220063810223671248e0,
          0.003393373439889186878847613140e0,
          0.000000000000000000000000000000e0};

//   Weights of the second null rule of degree 7.

static real W72[] = {-0.064293709240668260928898888457e0,
          0.003134665264639380635175608661e0,
          0.007822550509742830478456728602e0,
          0.048653051907689492781049400973e0,
          0.032883327334384971735434067029e0,
          -0.017019508374229390108580829589e0,
          0.025973557893399824586684707198e0,
          -0.010716753326806275930657622320e0,
          0.018315629578968063765722278290e0,
          -0.047607080313197299401024682666e0,
          -0.010716753326806275930657622320e0,
          0.018315629578968063765722278290e0,
          -0.047607080313197299401024682666e0};

//   Weights of the first degree 5 null rule.

static real W51[] = {0.021363205584741860993131879186e0,
          0.022716410154120323440432428315e0,
          -0.026366191271182090678117381002e0,
          0.029627021479068212693155637482e0,
          0.004782834546596399307634111034e0,
          0.004178667433984132052378990240e0,
          -0.065398996748953861618846710897e0,
          -0.033589813176131630980793760168e0,
          0.033018320112481615757912576257e0,
          0.012241086002709814125707333127e0,
          -0.033589813176131630980793760168e0,
          0.033018320112481615757912576257e0,
          0.012241086002709814125707333127e0};

//   Weights of the second degree 5 null rule.

static real W52[] = { -0.046058756832790538620830792345e0,
          0.005284159186732627192774759959e0,
          0.009325799301158899112648198129e0,
          -0.006101110360950124560783393745e0,
          -0.056223328794664871336486737231e0,
          -0.062516479198185693171971930698e0,
          0.022428226812039547178810743269e0,
          -0.000026014926110604563130107142e0,
          0.032882099937471182365626663487e0,
          0.018721740987705986426812755881e0,
          -0.000026014926110604563130107142e0,
          0.032882099937471182365626663487e0,
          0.018721740987705986426812755881e0};

//   Weights of first degree 3 null rule.

static real W31[] = {0.080867117677405246540283712799e0,
          -0.033915806661511608094988607349e0,
          0.014813362053697845461526433401e0,
          0.001442315416337389214102507204e0,
          -0.024309696484708683486455879210e0,
          -0.005135085639122398522835391664e0,
          -0.034649417896235909885490654650e0,
          0.035748423431577326597742956780e0,
          0.024548155266816447583155562333e0,
          -0.032897267038856299280541675107e0,
          0.035748423431577326597742956780e0,
          0.024548155266816447583155562333e0,
          -0.032897267038856299280541675107e0};

//   Weights of second degree 3 null rule.

static real W32[] = {-0.038457863913548248582247346193e0,
          -0.055143631258696406147982448269e0,
          -0.021536994314510083845999131455e0,
          0.001547467894857008228010564582e0,
          0.057409361764652373776043522086e0,
          -0.040636938884669694118908764512e0,
          -0.020801144746964801777584428369e0,
          0.019490770404993674256256421103e0,
          0.002606109985826399625043764771e0,
          0.023893703367437102825618048130e0,
          0.019490770404993674256256421103e0,
          0.002606109985826399625043764771e0,
          0.023893703367437102825618048130e0};

//   Weights of first degree 1 null rule.

static real W11[] = {0.074839568911184074117081012527e0,
          -0.004270103034833742737299816615e0,
          0.049352639555084484177095781183e0,
          0.048832124609719176627453278550e0,
          0.001042698696559292759051590242e0,
          -0.044445273029113458906055765365e0,
          -0.004670751812662861209726508477e0,
          -0.015613390485814379318605247424e0,
          -0.030581651696100000521074498679e0,
          0.010801113204340588798240297593e0,
          -0.015613390485814379318605247424e0,
          -0.030581651696100000521074498679e0,
          0.010801113204340588798240297593e0};

//   Weights of second degree 1 null rule.

static real W12[] = {0.009373028261842556370231264134e0,
          -0.074249368848508554545399978725e0,
          0.014709707700258308001897299938e0,
          0.009538502545163567494354463302e0,
          -0.014268362488069444905870465047e0,
          0.040126396495352694403045023109e0,
          0.028737181842214741174950928350e0,
          -0.031618075834734607275229608099e0,
          0.016879961075872039084307382161e0,
          0.010878914758683152984395046434e0,
          -0.031618075834734607275229608099e0,
          0.016879961075872039084307382161e0,
          0.010878914758683152984395046434e0};


void Triangle_Rule13::Apply(Integrand& F,Triangle& t,real& TheResult,real& TheError)
  {

  real Null71,Null72,Null51,Null52,Null31,Null32,Null11,Null12;
  const int Wtleng =13;
  real Tres = -1.0;
  real tune = 1.0;
  real f,Z1,Z2,Z3;
  real noise,r,r1,r2,r3,DEG1,DEG3,DEG5,DEG7;
  Point X[3],Center;
  int i,l;
  real  cvmax,cvmin,fmmax,fmmin,
            crival,facmed,facopt;

//
//  The abscissas are given in homogeneous coordinates.
//
// *FIRST EXECUTABLE STATEMENT DRLT2A
      cvmax = 0.5;
      cvmin = 0.9;
      crival = tune*cvmax + (1-tune)*cvmin;
      fmmax = 10;
      fmmin = 0.1;
      facmed = tune*fmmax + (1-tune)*fmmin;
      facopt = facmed/(crival*crival);

        if (Tres <= 0)
        {
        Tres=50*REAL_EPSILON;
        };


//   Compute contributions from the center of the triangle.

Center = (t.Vertex(0)+t.Vertex(1)+t.Vertex(2))/3;
f = F(Center);
TheResult = W[0]*f;
Null71 = W71[0]*f;
Null72 = W72[0]*f;
Null51 = W51[0]*f;
Null52 = W52[0]*f;
Null31 = W31[0]*f;
Null32 = W32[0]*f;
Null11 = W11[0]*f;
Null12 = W12[0]*f;


//   Compute contributions from points with
//   multiplicity 3.

for (i=1;i<Wtleng;i++)
  {
  Z1=G1[i];Z2=G2[i];Z3=1-Z1-Z2;
  X[0] = t.Vertex(0)*Z1+t.Vertex(1)*Z2+t.Vertex(2)*Z3;
  X[1] = t.Vertex(0)*Z2+t.Vertex(1)*Z3+t.Vertex(2)*Z1;
  X[2] = t.Vertex(0)*Z3+t.Vertex(1)*Z1+t.Vertex(2)*Z2;
  for (l=0;l<3;l++)
    {
    f = F(X[l]);
    TheResult += W[i]*f;
    Null71 += W71[i]*f;
    Null72 += W72[i]*f;
    Null51 += W51[i]*f;
    Null52 += W52[i]*f;
    Null31 += W31[i]*f;
    Null32 += W32[i]*f;
    Null11 += W11[i]*f;
    Null12 += W12[i]*f;
    };
};
//    No differences are computed;

//    Compute errors.

          noise = fabs(TheResult)*Tres;
          DEG7 = sqrt(sqr(Null71)+sqr(Null72));
          if (DEG7 <= noise)
            {
            TheError = noise;
            }
          else
            {
            DEG5 = sqrt(sqr(Null51)+sqr(Null52));
            DEG3 = sqrt(sqr(Null31)+sqr(Null32));
            DEG1 = sqrt(sqr(Null11)+sqr(Null12));
            if(DEG5 != 0)
              {
              r1=DEG7/DEG5;
              }
            else
              {
              r1 = 1;
              };
            if(DEG3 != 0)
              {
              r2=DEG5/DEG3;
              }
            else
              {
              r2 = 1;
              };
            if(DEG1 != 0)
              {
              r3=DEG3/DEG1;
              }
            else
              {
              r3 = 1;
              };
            r=max(r1,max(r2,r3));
            if (r>= 1)
              {
              TheError =10*max(max(max(DEG1,DEG3),DEG5),DEG7);
              }
            else
              {
              if (r>=crival)
                {
                TheError = facmed*r*DEG7;
                }
              else
                {
                TheError = facopt *r*r*r*DEG7;
                };
              };
            TheError = max(noise,TheError);
            };
          TheError *= t.Volume();
          TheResult *=t.Volume();
          TheError = min(fabs(TheResult),TheError);

// **END DRLTRI

  }
/////////////////////////////////////////////////////////

void Triangle_Rule13::ComputeDiffs(Integrand& F,Triangle& t,Vector<real>& Diff)
  {

  Point Centre = (t.Vertex(0)+t.Vertex(1)+t.Vertex(2))/3;
  real F0 = F(Centre);  
  real F1,F3;
  Vector<Point> Dir(3);
  Vector<real>  Angle(3);  

  Dir[0] = t.Vertex(2)-t.Vertex(1);
  Dir[1] = t.Vertex(2)-t.Vertex(0);
  Dir[2] = t.Vertex(1)-t.Vertex(0);

  Angle[0] = acos(Dir[1]*Dir[2]/(Dir[1].Length()*Dir[2].Length()));   
  Angle[1] = acos(-Dir[2]*Dir[0]/(Dir[2].Length()*Dir[0].Length()));  
  Angle[2] = acos(Dir[0]*Dir[1]/(Dir[0].Length()*Dir[1].Length()));   

  // Calculation of differences 

  for (int d=0 ; d<=2 ; d++)
    {
    F1 = F(Centre+2*Dir[d]/15) + F(Centre-2*Dir[d]/15);
    F3 = F(Centre+4*Dir[d]/15) + F(Centre-4*Dir[d]/15);
    Diff[d] = norm(Dir[d],1) * fabs(6*F0-4*F1+F3); 
    }

  // Test for abnormal triangles  

  const real Eps = REAL_EPSILON;  
  const real Tol = 1e-8;    

    // Test wether one of the angles is close to  PI  
 
    if (fabs(M_PI-Angle[0])<Tol) 
      {  Diff[0] = 1.0;  Diff[1] = Eps;  Diff[2] = Eps;  }  
    if (fabs(M_PI-Angle[1])<Tol) 
      {  Diff[0] = Eps;  Diff[1] = 1.0;  Diff[2] = Eps;  } 
    if (fabs(M_PI-Angle[2])<Tol) 
      {  Diff[0] = Eps;  Diff[1] = Eps;  Diff[2] = 1.0;  } 
   
   // Test wether two of the angles are near to  PI/2  
 
   if ((fabs(M_PI_2-Angle[0])<Tol)&&(fabs(M_PI_2-Angle[1])<Tol))   
     {  Diff[2] *= Eps;  } 
   if ((fabs(M_PI_2-Angle[0])<Tol)&&(fabs(M_PI_2-Angle[2])<Tol))    
     {  Diff[1] *= Eps;  } 
   if ((fabs(M_PI_2-Angle[1])<Tol)&&(fabs(M_PI_2-Angle[2])<Tol))    
     {  Diff[0] *= Eps;  }  
   

  // Calculation of largest difference 
  // Then we divide the differences by the 1-norm of the side 
  // belonging to this largest one.  In this way we make the  
  // value of 'difftreshold' in the processor less size-dependent.   
 
  int nrside=0; 
  real maxdiff = Diff[0]; 

  if (Diff[1]>maxdiff)  { maxdiff = Diff[1]; nrside = 1; }  
  if (Diff[2]>maxdiff)  { maxdiff = Diff[2]; nrside = 2; }   
  
  real scalefactor = norm(Dir[nrside],1);  
  for (int ll=0 ; ll<=2 ; ll++) 
    { 
      Diff[ll] /= scalefactor;   
    } 
  
  }

/////////////////////////////////////////////////////////

void Triangle_Rule13::ApplyWithDiffs(Integrand& F,Triangle& t,real& TheResult,
				     real& TheError,Vector<real>& D)
  {
  Apply(F,t,TheResult,TheError);
//  ComputeDiffs(F,t,D); 
  }

/////////////////////////////////////////////////////////
Triangle_Rule13::Triangle_Rule13()
    :Rule<Triangle>()
    {
    }
/////////////////////////////////////////////////////////




