
//O'Hara etal 2011 PLoS Comp Biology
// ORd human model
//June-1-2019
//Pei-Chi Yang
//Colleen Clancy Lab
//Ikr Drug model - Mox
// To simulated Figure 5 (E) in the paper: A computational pipeline to predict cardiotoxicity:
// From the atom to the rhythm


#include <iostream>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>

using namespace std;

// %extracellular ionic concentrations
const double nao = 140.0;
const double cao = 1.8;
const double ko = 5.4;

// %physical constants
const double R = 8314.0;
const double T = 310.0;
const double F = 96485.0;

// %cell geometry
const double L = 0.01;
const double rad = 0.0011;
const double vcell = 1000*3.14*rad*rad*L;
const double Ageo = 2*3.14*rad*rad+2*3.14*rad*L;
const double Acap = 2*Ageo;
const double vmyo = 0.68*vcell;
const double vnsr = 0.0552*vcell;
const double vjsr = 0.0048*vcell;
const double vss = 0.02*vcell;


//// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//const int celltype = 0; //Cell type = (0) for endo; (1) epi (2) for M,

const double Drug= 2.5;
const int drugname = Drug;

const double BCL = 1000;

const int stimuli = 4003;



const int tl= 165;  // length of the tissue
const int tw= 1;  // width of the tissue

const double base_dt = 0.005;
const int fold = 1.0/base_dt;

const char *FromSSstate =  "1Dstate";
const char *stateFileName =  "1Dstate.yang";//"1Dstate.Dof2_1Hz";


const double dx = 0.01;
const double dy = 0.01;
const double gj_use = 2.5; //uS

const double Rmyo = 150.0;       //ohm*cm
const double Rg = 3.14159*pow(rad,2)/(gj_use*1.0e-6);
const double Df = (1e3*rad)/(4.0*(Rmyo+Rg/dx)) ;//0.00092;


typedef struct cell {
	double CaMKa, CaMKt;
	double m, j, hsp, hp;
	double hf, hs, h, jp;
	double mL, hL, hLp;
	double d, ff, fs, f, fcaf, fcas, fca, jca;
	double ffp, fp, fcafp, fcap, nca, ap;
	double xs1, xs2, xr, xrf, xrs, xk1;
	double a, iF, iS, i, iFp, iSp, ip;
	double nai, nass, ki, kss;
	double cai, cass, cansr, cajsr;
	double Jrelp, Jrelnp;
	double v, v_new, dvdt;
    double I_Kr, C3_Kr, C2_Kr, O_Kr, C1_Kr;
    
    double In_drug, C3n_drug, C2n_drug, On_drug, C1n_drug;
    double Ic_drug, C3c_drug, C2c_drug, Oc_drug, C1c_drug;
    double zw_O_drug , zw_C1_drug , zw_C2_drug , zw_C3_drug , zw_I_drug;

    int celltype;
    double GKr;
} Cell;

typedef struct simState {
	double t, tt;
	int tstep, counter, beat;
	Cell cellData[tw][tl];
} SimState;

SimState theState;
SimState *S = &theState;

double box_muller(double m, double s);
double Calcu_I_Total(Cell *theCell, double dt, double I_inj);


int main () {
	
	char name[30];
	FILE *output;
    FILE *output1;
	FILE *ecg;
	string str;
    
	double Ev;
	int ww;
	int ll;
	double V;
	Cell *theCell;
	double dt = base_dt;
	double I_Total;
	double dv;
	double cycle_length;
	double cv1;
	double I_inj;
	double csc, randomN;
    
	int done = 0;
	
	omp_set_num_threads( 56 );
	
	time_t startTime;
	time_t previousTime;
	
	const time_t timeSave = 1*60*60;
	startTime = time(NULL);
	previousTime = startTime;
    
    srand48(startTime);
    str = "_Mox";
    
    double waitTime = 1002*1e3 +550;
    
    for (int subjectCase = 1; subjectCase <= 1; subjectCase++)  {
        
        
      
        
        //	output=fopen("data_fiber.txt", "w");
        //	ecg = fopen("data_ecg.txt", "w");
        
        double t1c1 = 1E10;
        double t1c2 = 1E10;
        
        
        
        FILE *fp = fopen(FromSSstate, "r");
        
        //FILE *fp5 = fopen( "gKr_f4.txt", "r" );
        
        if (fp == NULL) {
            cout << "New Start" << endl;
            
            ww = 0;
            S->counter=0;
            
            //// Initial conditions
            
            for (ll=0; ll<=(tl-1); ll+=1){
                
                theCell = &(S->cellData[ww][ll]);
                
                
                
                //// Initial conditions
                theCell->v_new=-87.;
                theCell->nai=7.;
                theCell->nass=theCell->nai;
                theCell->ki=145.;
                theCell->kss=theCell->ki;
                theCell->cai=1.0e-4;
                theCell->cass=theCell->cai;
                theCell->cansr=1.2;
                theCell->cajsr=theCell->cansr;
                theCell->m=0.;
                theCell->hf=1.;
                theCell->hs=1.;
                theCell->j=1.;
                theCell->hsp=1.;
                theCell->jp=1.;
                theCell->mL=0.;
                theCell->hL=1.;
                theCell->hLp=1.;
                theCell->a=0.;
                theCell->iF=1.;
                theCell->iS=1.;
                theCell->ap=0.;
                theCell->iFp=1.;
                theCell->iSp=1.;
                theCell->d=0.;
                theCell->ff=1.;
                theCell->fs=1.;
                theCell->fcaf=1.;
                theCell->fcas=1.;
                theCell->jca=1.;
                theCell->nca=0.;
                theCell->ffp=1.;
                theCell->fcafp=1.;
                theCell->xrf=0.;
                theCell->xrs=0.;
                theCell->xs1=0.;
                theCell->xs2=0.;
                theCell->xk1=1.;
                theCell->Jrelnp=0.;
                theCell->Jrelp=0.;
                theCell->CaMKt=0.;
                theCell->CaMKa=0.;
                // IKr initials
                theCell->I_Kr = 0;
                theCell->C3_Kr = 1;
                theCell->C2_Kr = 0;
                theCell->O_Kr = 0;
                
                theCell->C1_Kr = 1-(theCell->I_Kr + theCell->C3_Kr + theCell->C2_Kr + theCell->O_Kr +theCell->In_drug +theCell->C3n_drug+theCell->C2n_drug+theCell->C1n_drug+theCell->C3c_drug+theCell->C2c_drug+theCell->C1c_drug+theCell->Ic_drug + theCell->zw_I_drug + theCell->zw_C3_drug + theCell->zw_C2_drug + theCell->zw_C1_drug + theCell->zw_O_drug);
                
                theCell->In_drug = 0;
                theCell->C3n_drug = 0;
                theCell->C2n_drug = 0;
                theCell->On_drug = 0;
                theCell->C1n_drug = 0;
                
                theCell->Ic_drug = 0;
                theCell->C3c_drug = 0;
                theCell->C2c_drug = 0;
                theCell->Oc_drug = 0;
                theCell->C1c_drug = 0;
                
                theCell->zw_I_drug = 0;
                theCell->zw_C3_drug = 0;
                theCell->zw_C2_drug = 0;
                theCell->zw_O_drug = 0;
                theCell->zw_C1_drug = 0;

                //   fscanf( fp5, "%lf", &(theCell->GKr));
                theCell->GKr = 1.0 * ( ( 1 * ( 0.05 - 0.04 ) / (tl-1) * ll )
                                      + 0.04 );
                
                if (ll <= 80 ) { //60
                    theCell->celltype = 0;
                    //   } else if ( ll >= 61 && ll <= 105) { //ll >= 61 && ll <= 105
                    //       theCell->celltype = 2;
                } else {
                    theCell->celltype = 1;
                }
            }
            S->beat=1;
            S->t = 0.0;
            S->tt = 0.0;
            S->tstep = 0;
            S->counter = 0;
            
            sprintf(name, ("vmN%d"+str+"%d.txt").c_str(),subjectCase,drugname);
            output = fopen(name, "w");
            cout << name << endl;
            
            sprintf(name, ("beatBCLN%d"+str+"%d.txt").c_str(),subjectCase,drugname);
            output1 = fopen(name, "w");
            cout << name << endl;
            
            sprintf(name, ("ECGN%d"+str+"%d.txt").c_str(),subjectCase,drugname);
            ecg=fopen ( name, "w");
            cout << name << endl;
            
            
        } else {
            fread(S, sizeof(SimState), 1, fp);
            cout << "restarting from step " << S->counter << endl;
            fclose(fp);
            
            cout << "N = " << subjectCase << endl;
            
            sprintf(name, ("vmN%d"+str+"%d.txt").c_str(),subjectCase,drugname);
            output = fopen(name, "w");
            cout << name << endl;
            
            sprintf(name, ("beatBCLN%d"+str+"%d.txt").c_str(),subjectCase,drugname);
            output1 = fopen(name, "w");
            cout << name << endl;
            
            sprintf(name, ("ECGN%d"+str+"%d.txt").c_str(),subjectCase,drugname);
            ecg=fopen ( name, "w");
            cout << name << endl;
            
        }
        
        
        
        
        done = 0;
        while (!done) {
            
            ww=0;
            
            if (S->beat <= 4000) {
                cycle_length = BCL;
            } else if (S->beat == 4001){
                cycle_length = BCL - 200; //800; (s1-s2)
            } else if (S->beat == 4002){
                cycle_length = BCL + 4000 ;//5000; (s2-s3)
        
            } else {
                cycle_length = 2000; //for last beat
            }
            
            
            //randomN = box_muller(0,1);
            //csc = 0.3 * randomN*sqrt(dt);
            
#pragma omp parallel for private( ll, theCell)
            
            for (ll=0; ll<=(tl-1); ll+=1){
                theCell = &(S->cellData[ww][ll]);
                theCell->v = theCell->v_new;
            }
            
#pragma omp parallel for private( ll, theCell, I_inj, dv, V, I_Total)
            
            
            for (ll=0; ll<=(tl-1); ll+=1){
                
                theCell = &(S->cellData[ww][ll]);
                
                if ( S->t < 0.5 &&  (ll==0 || ll==1) ) {
                    I_inj = -300.0;
               
                } else {
                    I_inj=0.0;
                }
                
               // randomN = box_muller(0,1);
                csc = 0; //0.3 * randomN*sqrt(dt);
                
                V=theCell->v;
                
                I_Total=Calcu_I_Total(theCell, dt, I_inj);
                
                
                
                if(ll>0 && ll<(tl-1)) { dv=dt*(-I_Total+Df*(S->cellData[ww][ll-1].v - 2*theCell->v + S->cellData[ww][ll+1].v )/(dx*dx) ) +csc  ; }
                else if (ll==0) { dv=dt*(-I_Total+Df*(-theCell->v + S->cellData[ww][ll+1].v )/(dx*dx)) + csc ; }
                else if (ll==(tl-1)) { dv=dt*(-I_Total+Df*(-theCell->v + S->cellData[ww][ll-1].v )/(dx*dx)) + csc ; }
                
                theCell->dvdt=fabs(dv/dt);
                theCell->v_new = theCell->v+dv;
                
                if ( ll == 49) {
                    V = theCell->v;
                    if ( V > 0 ) {
                        if (t1c1 > S->t ) {
                            t1c1 = S->t; // 1st beat arrived.
                            cout << " 1st beat arrived." << endl;
                        }
                        
                    }
                    
                } else if ( ll == 51 ) {
                    V = theCell->v;
                    if ( V > 0 ) {
                        if (t1c2 > S->t ) {
                            t1c2 = S->t; // 1st beat arrived.
                        }
                        
                    }
                }
            } // End for loop
            
            cv1 = dx * 1000* ( 51 - 49 ) / ( t1c2 - t1c1 ) ;
            
            
            if(  S->beat> 4000 && ( (S->counter) % (fold*1) == 0 ) ) {
                
                fprintf (output, "%10.f", S->tt);
                for (ll = 0; ll < tl; ll += 1) {
                    fprintf (output, "\t%8.2f", S->cellData[ww][ll].v);
                }
                fprintf (output, "\n");
                
                Ev = 0;
                for (ll=15; ll<=(tl-16); ll++){
                    Ev = Ev + ( S->cellData[ww][ll].v - S->cellData[ww][ll+1].v )*( 1 / ((tl - ll - 1)*dx+2)-1/((tl - ll)*dx+2));
                }
                fprintf(ecg, "%10.3f\t%10.6f\t%10.6f\n", S->tt, Ev, cycle_length);
            }
            
            if (S->counter % (fold*100) == 0) {
                cout<< "tt: " << S->tt << ", runtime: " << (time(NULL) - startTime)/60 << " min " << ", tstep: " << S->tstep << ", counter; " << S->counter << endl;
            }
            
            
            
            S->t += dt;
            S->tt += dt;
            S->counter += 1;
            
            
            
            if(S->t >= cycle_length) {
                fprintf (output1, "%d\t%8.5f\n", S->beat, cycle_length);
                S->t = 0;
                S->beat++;
                
                cout << "now the beat is: " << S->beat << endl;
                cout << "CV = " << cv1 << endl;
                cout << "cycle length = " << cycle_length << endl;
            }
            
            
            if (S->beat > stimuli) { done = 1;}
            
            
            
        } // while done
        
        waitTime +=100;
        
        fclose ( output );
        fclose ( output1 );
        fclose (ecg);
        
    }
    
    S->counter=0;
    
    S->beat=1;
    S->t = 0.0;
    S->tt = 0.0;
    
    ///---------- save for initial values-------------///
    FILE *fp2 = fopen(stateFileName, "w");
    fwrite(S, sizeof(SimState), 1, fp2);
    fclose(fp2);
    
    return 0;
}

/* normal random variate generator */
double box_muller(double m, double s) { // mean m, standard deviation s //

	double x1, x2, w, y1;
    double y2 = 0;
	int use_last = 0;


	if (use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		do {
			x1 = 2.0 * drand48() - 1.0;
			x2 = 2.0 * drand48() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	return( m + y1 * s );
}

// total current
double Calcu_I_Total(Cell *theCell, double dt, double I_inj){
	double v = theCell->v;
	
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	
	// %CaMK constants
	const double KmCaMK=0.15;
	
	const double aCaMK=0.05;
	const double bCaMK=0.00068;
	const double CaMKo=0.05;
	const double KmCaM=0.0015;
	// %update CaMK
	const double CaMKb=CaMKo*(1.0-theCell->CaMKt)/(1.0+KmCaM/theCell->cass);
	//const double CaMKa=CaMKb+CaMKt;
	theCell->CaMKa=CaMKb+theCell->CaMKt;
	const double dCaMKt=aCaMK*CaMKb*(CaMKb+theCell->CaMKt)-bCaMK*theCell->CaMKt;
	theCell->CaMKt = theCell->CaMKt + dt*dCaMKt;
	
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	
	// %reversal potentials
	double ENa=(R*T/F)*log(nao/theCell->nai);
	double EK=(R*T/F)*log(ko/theCell->ki);
	double PKNa=0.01833;
	double EKs=(R*T/F)*log((ko+PKNa*nao)/(theCell->ki+PKNa*theCell->nai));
	
	// %convenient shorthand calculations
	double vffrt=v*F*F/(R*T);
	double vfrt=v*F/(R*T);
	
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	
	
	// %calculate INa
	
	double mss=1.0/(1.0+exp((-(v+39.57))/9.871));
	double tm=1.0/(6.765*exp((v+11.64)/34.77)+8.552*exp(-(v+77.42)/5.955));
	//double dm=(mss-m)/tm;
	theCell->m = mss-(mss-theCell->m)*exp(-dt/tm);
	
	double hss=1.0/(1+exp((v+82.90)/6.086));
	double thf=1.0/(1.432e-5*exp(-(v+1.196)/6.285)+6.149*exp((v+0.5096)/20.27));
	double ths=1.0/(0.009794*exp(-(v+17.95)/28.05)+0.3343*exp((v+5.730)/56.66));
	double Ahf=0.99;
	double Ahs=1.0-Ahf;
	//double dhf=(hss-hf)/thf;
	//double dhs=(hss-hs)/ths;
	theCell->hf = hss-(hss-theCell->hf)*exp(-dt/thf);
	theCell->hs = hss-(hss-theCell->hs)*exp(-dt/ths);
	theCell->h = Ahf * theCell->hf + Ahs * theCell->hs;
	
	double jss=hss;
	double tj=2.038+1.0/(0.02136*exp(-(v+100.6)/8.281)+0.3052*exp((v+0.9941)/38.45));
	//double dj=(jss-j)/tj;
	theCell->j = jss-(jss-theCell->j)*exp(-dt/tj);
	
	double hssp=1.0/(1+exp((v+89.1)/6.086));
	double thsp=3.0*ths;
	//double dhsp=(hssp-hsp)/thsp;
	theCell->hsp = hssp-(hssp-theCell->hsp)*exp(-dt/thsp);
	theCell->hp=Ahf*theCell->hf+Ahs*theCell->hsp;
	
	double tjp=1.46*tj;
	//double djp=(jss-jp)/tjp;
	theCell->jp = jss-(jss-theCell->jp)*exp(-dt/tjp);
	
	double GNa=75.;
	double fINap=(1.0/(1.0+KmCaMK/theCell->CaMKa));
	
	double INa=GNa*(v-ENa)*theCell->m*theCell->m*theCell->m*((1.0-fINap)*theCell->h*theCell->j+fINap*theCell->hp*theCell->jp);
	
	// %calculate INaL
	double mLss=1.0/(1.0+exp((-(v+42.85))/5.264));
	double tmL=1.0/(6.765*exp((v+11.64)/34.77)+8.552*exp(-(v+77.42)/5.955));
	//double dmL=(mLss-mL)/tmL;
	theCell->mL = mLss-(mLss-theCell->mL)*exp(-dt/tmL);
	
	double hLss=1.0/(1.0+exp((v+87.61)/7.488));
	double thL=200.0;
	//double dhL=(hLss-hL)/thL;
	theCell->hL = hLss-(hLss-theCell->hL)*exp(-dt/thL);
	
	double hLssp=1.0/(1.0+exp((v+93.81)/7.488));
	double thLp=3.0*thL;
	//double dhLp=(hLssp-hLp)/thLp;
	theCell->hLp = hLssp-(hLssp-theCell->hLp)*exp(-dt/thLp);
	
	double GNaL=0.0075;
	if( theCell->celltype==1 ) {
		GNaL=GNaL*0.6;
	}
	double fINaLp=(1.0/(1.0+KmCaMK/theCell->CaMKa));
	
	double INaL=GNaL*(v-ENa)*theCell->mL*((1.0-fINaLp)*theCell->hL+fINaLp*theCell->hLp);
	
	// %calculate Ito
	double ass=1.0/(1.0+exp((-(v-14.34))/14.82));
	double ta=1.0515/(1.0/(1.2089*(1.0+exp(-(v-18.4099)/29.3814)))+3.5/(1.0+exp((v+100.0)/29.3814)));
	//double da=(ass-a)/ta;
	theCell->a = ass - (ass - theCell->a)*exp(-dt/ta);
	
	double iss=1.0/(1.0+exp((v+43.94)/5.711));
	double delta_epi;
	if( theCell->celltype==1 ) {
		delta_epi=1.0-(0.95/(1.0+exp((v+70.0)/5.0)));
	} else {
		delta_epi=1.0;
	}
	double tiF=4.562+1/(0.3933*exp((-(v+100.0))/100.0)+0.08004*exp((v+50.0)/16.59));
	double tiS=23.62+1/(0.001416*exp((-(v+96.52))/59.05)+1.780e-8*exp((v+114.1)/8.079));
	tiF=tiF*delta_epi;
	tiS=tiS*delta_epi;
	double AiF=1.0/(1.0+exp((v-213.6)/151.2));
	double AiS=1.0-AiF;
	//double diF=(iss-iF)/tiF;
	//double diS=(iss-iS)/tiS;
	theCell->iF = iss - (iss - theCell->iF)*exp(-dt/tiF);
	theCell->iS = iss - (iss - theCell->iS)*exp(-dt/tiS);
	theCell->i = AiF * theCell->iF + AiS * theCell->iS;
	
	double assp=1.0/(1.0+exp((-(v-24.34))/14.82));
	//double dap=(assp-ap)/ta;
	theCell->ap = assp - (assp - theCell->ap)*exp(-dt/ta);
	
	double dti_develop=1.354+1.0e-4/(exp((v-167.4)/15.89)+exp(-(v-12.23)/0.2154));
	double dti_recover=1.0-0.5/(1.0+exp((v+70.0)/20.0));
	double tiFp=dti_develop*dti_recover*tiF;
	double tiSp=dti_develop*dti_recover*tiS;
	//double diFp=(iss-iFp)/tiFp;
	//double diSp=(iss-iSp)/tiSp;
	theCell->iFp = iss - (iss - theCell->iFp)*exp(-dt/tiFp);
	theCell->iSp = iss - (iss - theCell->iSp)*exp(-dt/tiSp);
	theCell->ip = AiF * theCell->iFp + AiS * theCell->iSp;
	
	double Gto=0.02;
	if( theCell->celltype==1 ){
		Gto=Gto*4.0;
	} else if( theCell->celltype==2 ){
		Gto=Gto*4.0;
	}
	double fItop=(1.0/(1.0+KmCaMK/theCell->CaMKa));
	
	double Ito=Gto*(v-EK)*((1.0-fItop)*theCell->a*theCell->i+fItop*theCell->ap*theCell->ip);
	
	// %calculate ICaL, ICaNa, ICaK
	double dss=1.0/(1.0+exp((-(v+3.940))/4.230));
	double td=0.6+1.0/(exp(-0.05*(v+6.0))+exp(0.09*(v+14.0)));
	//double dd=(dss-d)/td;
	theCell->d = dss-(dss-theCell->d)*exp(-dt/td);
	
	double fss=1.0/(1.0+exp((v+19.58)/3.696));
	double tff=7.0+1.0/(0.0045*exp(-(v+20.0)/10.0)+0.0045*exp((v+20.0)/10.0));
	double tfs=1000.0+1.0/(0.000035*exp(-(v+5.0)/4.0)+0.000035*exp((v+5.0)/6.0));
	double Aff=0.6;
	double Afs=1.0-Aff;
	//double dff=(fss-ff)/tff;
	//double dfs=(fss-fs)/tfs;
	theCell->ff = fss-(fss-theCell->ff)*exp(-dt/tff);
	theCell->fs = fss-(fss-theCell->fs)*exp(-dt/tfs);
	theCell->f = Aff * theCell->ff + Afs * theCell->fs;
	
	double fcass=fss;
	double tfcaf=7.0+1.0/(0.04*exp(-(v-4.0)/7.0)+0.04*exp((v-4.0)/7.0));
	double tfcas=100.0+1.0/(0.00012*exp(-v/3.0)+0.00012*exp(v/7.0));
	double Afcaf=0.3+0.6/(1.0+exp((v-10.0)/10.0));
	double Afcas=1.0-Afcaf;
	//double dfcaf=(fcass-fcaf)/tfcaf;
	//double dfcas=(fcass-fcas)/tfcas;
	theCell->fcaf = fcass - (fcass - theCell->fcaf)*exp(-dt/tfcaf);
	theCell->fcas = fcass - (fcass - theCell->fcas)*exp(-dt/tfcas);
	theCell->fca = Afcaf * theCell->fcaf + Afcas * theCell->fcas;
	
	double tjca=75.0;
	//double djca=(fcass-jca)/tjca;
	theCell->jca = fcass - (fcass - theCell->jca)*exp(-dt/tjca);
	
	double tffp=2.5*tff;
	//double dffp=(fss-ffp)/tffp;
	theCell->ffp = fss - (fss - theCell->ffp)*exp(-dt/tffp);
	theCell->fp = Aff * theCell->ffp + Afs * theCell->fs;
	
	double tfcafp=2.5*tfcaf;
	//double dfcafp=(fcass-fcafp)/tfcafp;
	theCell->fcafp = fcass - (fcass - theCell->fcafp)*exp(-dt/tfcafp);
	theCell->fcap = Afcaf * theCell->fcafp + Afcas * theCell->fcas;
	
	double Kmn=0.002;
	double k2n=1000.0;
	double km2n=theCell->jca*1.0;
	double anca=1.0 / ( k2n/km2n+ pow((1.0+Kmn/theCell->cass),4) );
	//double dnca=anca*k2n-nca*km2n;
	theCell->nca = anca*(k2n/km2n)-(anca*(k2n/km2n)-theCell->nca)*exp(-km2n*dt);
	
	double PhiCaL=4.0*vffrt*(theCell->cass*exp(2.0*vfrt)-0.341*cao)/(exp(2.0*vfrt)-1.0);
	double PhiCaNa=1.0*vffrt*(0.75*theCell->nass*exp(1.0*vfrt)-0.75*nao)/(exp(1.0*vfrt)-1.0);
	double PhiCaK=1.0*vffrt*(0.75*theCell->kss*exp(1.0*vfrt)-0.75*ko)/(exp(1.0*vfrt)-1.0);
	double zca=2.0;
	double PCa=0.0001;
	if( theCell->celltype==1 ) {
		PCa=PCa*1.2;
	} else if( theCell->celltype==2 ) {
		PCa=PCa*2.5;
	}
	
	double PCap=1.1*PCa;
	double PCaNa=0.00125*PCa;
	double PCaK=3.574e-4*PCa;
	double PCaNap=0.00125*PCap;
	double PCaKp=3.574e-4*PCap;
	double fICaLp=(1.0/(1.0+KmCaMK/theCell->CaMKa));
	double ICaL=(1.0-fICaLp)*PCa*PhiCaL*theCell->d*(theCell->f*(1.0-theCell->nca)+theCell->jca*theCell->fca*theCell->nca)+fICaLp*PCap*PhiCaL*theCell->d*(theCell->fp*(1.0-theCell->nca)+theCell->jca*theCell->fcap*theCell->nca);
	double ICaNa=(1.0-fICaLp)*PCaNa*PhiCaNa*theCell->d*(theCell->f*(1.0-theCell->nca)+theCell->jca*theCell->fca*theCell->nca)+fICaLp*PCaNap*PhiCaNa*theCell->d*(theCell->fp*(1.0-theCell->nca)+theCell->jca*theCell->fcap*theCell->nca);
	double ICaK=(1.0-fICaLp)*PCaK*PhiCaK*theCell->d*(theCell->f*(1.0-theCell->nca)+theCell->jca*theCell->fca*theCell->nca)+fICaLp*PCaKp*PhiCaK*theCell->d*(theCell->fp*(1.0-theCell->nca)+theCell->jca*theCell->fcap*theCell->nca);
	
	
	
	
    
    // Markov IKr for Mox
    double sum2;
    double hh = dt;
    double ae=exp(24.335+0.0112*v-25.914);
    double be=exp(13.688-0.0603*v-15.707);
    double ai=exp(30.061-0.0312*v-33.243);
    double ain=exp(22.746-25.914);
    double bin=exp(13.193-15.707);
    double bi=exp(30.016+0.0223*v-30.888)*pow((5.4/ko),0.4);   //inactivation
    double aa=exp(22.098+0.0365*v-25.914);  //activation
    double bb=exp(7.313-0.0399*v-15.707);  //deactivation
    
    double bid_zw = 0*bi;
    
    double bi_d = 0*bi ;
    
    double bid = 0*bi ;
    
    
    
    const double portionZ = 0.8399;
    
    const double portionC = 0.1033;
    
    const double portion = 0.0568;
    
   
    double Mox = Drug / 401.431*1E3; // Moxifloxacin / Molar mass) convert to uM
    
    double drug_charged = Mox *  portionC;
    
    double drug_zw = Mox *  portionZ;
    
    double drug_neutral = Mox * portion;
    
    
    //open
    
    double Kd_neu_open = 0.7408; //uM
    
    double Kd_cha_open = 6690; //uM
    
    double Kd_zw_open = 8550;// //uM Mox(+/-) US02
    
    
    double kOD, kID, rOD, rID, kO_D, kI_D, rO_D, rI_D, aid, ai_d, aid_zw, kOD_zw, kID_zw, rOD_zw, rID_zw;
    
    double Inputs[] = { 0.659204, 0.421146, 0.337106 };
    
    if (Drug == 0 || drug_charged == 0 || drug_neutral == 0 || drug_zw == 0) {
        
        kO_D = 0;
        kI_D = 0;
        rO_D = 0;
        rI_D = 0;
        ai_d = 0;
        
        kOD = 0;
        kID = 0;
        rOD = 0;
        rID = 0;
        aid = 0;
        
        kOD_zw = 0;
        kID_zw = 0;
        rOD_zw = 0;
        rID_zw = 0;
        aid_zw = 0;
        
    } else {
        
        // neutral drug
        kO_D = Inputs[0] * drug_neutral;//nM
        kI_D = 0;
        rO_D = Inputs[0] * Kd_neu_open;
        rI_D = 0;
        ai_d = 0;
        
        // charged drug
        kOD = Inputs[1] * drug_charged;//nM
        kID = 0;
        rOD = Inputs[1] * Kd_cha_open;
        rID = 0;
        aid = 0;
        
        // zwitterionic drug !!! need to add & modify
        kOD_zw = Inputs[2] * drug_zw;//nM
        kID_zw = 0;
        rOD_zw = Inputs[2] * Kd_zw_open;
        rID_zw = 0;
        aid_zw = 0;
        
        
        
        
    }
    
    double C3 = theCell->C3_Kr;
    double C2 = theCell->C2_Kr;
    double C1  = theCell->C1_Kr;
    double O  = theCell->O_Kr;
    double I  = theCell->I_Kr;
    
    double C3_o  = theCell->C3_Kr;
    double C2_o  = theCell->C2_Kr;
    double C1_o  = theCell->C1_Kr;
    double O_o  = theCell->O_Kr;
    double I_o  = theCell->I_Kr;
    
    //Charged Drug States
    double DC3  = theCell->C3c_drug;
    double DC2  = theCell->C2c_drug;
    double DC1  = theCell->C1c_drug;
    double DO  = theCell->Oc_drug;
    double DI  = theCell->Ic_drug;
    
    double DC3_o  = theCell->C3c_drug;
    double DC2_o  = theCell->C2c_drug;
    double DC1_o  = theCell->C1c_drug;
    double DO_o  = theCell->Oc_drug;
    double DI_o  = theCell->Ic_drug;
    
    //Neutral Drug States
    double D_C3  = theCell->C3n_drug;
    double D_C2  = theCell->C2n_drug;
    double D_C1  = theCell->C1n_drug;
    double D_O  = theCell->On_drug;
    double D_I  = theCell->In_drug;
    
    double D_C3_o  = theCell->C3n_drug;
    double D_C2_o  = theCell->C2n_drug;
    double D_C1_o  = theCell->C1n_drug;
    double D_O_o  = theCell->On_drug;
    double D_I_o = theCell->In_drug;
    
    //zwitterionic Drug States
    double Dzw_C3 = theCell->zw_C3_drug;
    double Dzw_C2 = theCell->zw_C2_drug;
    double Dzw_C1 = theCell->zw_C1_drug;
    double Dzw_O = theCell->zw_O_drug;
    double Dzw_I = theCell->zw_I_drug;
    
    double Dzw_C3_o = theCell->zw_C3_drug;
    double Dzw_C2_o = theCell->zw_C2_drug;
    double Dzw_C1_o = theCell->zw_C1_drug;
    double Dzw_O_o = theCell->zw_O_drug;
    double Dzw_I_o = theCell->zw_I_drug;
    
    const double co_O  = 1. / ( 1 + hh * ( bi + bb + kO_D + kOD + kOD_zw ) );
    const double co_C1 = 1. / ( 1 + hh * ( aa + bin ) );
    const double co_C2 = 1. / ( 1 + hh * ( ain + be ) );
    const double co_C3 = 1. / ( 1 + hh * ( ae ) );
    const double co_I  = 1. / ( 1 + hh * ( ai + kI_D + kID + kID_zw ) );
    
    //Charged Drug States
    const double co_DO  = 1. / ( 1 + hh * ( bid + bb + rOD ) );
    const double co_DC1 = 1. / ( 1 + hh * ( aa + bin ) );
    const double co_DC2 = 1. / ( 1 + hh * ( ain + be ) );
    const double co_DC3 = 1. / ( 1 + hh * ( ae ) );
    const double co_DI  = 1. / ( 1 + hh * ( aid + rID ) );
    
    //zwitterionic Drug States
    const double co_Dzw_O  = 1. / ( 1 + hh * ( bid_zw + bb + rOD_zw ) );
    const double co_Dzw_C1 = 1. / ( 1 + hh * ( aa + bin ) );
    const double co_Dzw_C2 = 1. / ( 1 + hh * ( ain + be ) );
    const double co_Dzw_C3 = 1. / ( 1 + hh * ( ae ) );
    const double co_Dzw_I  = 1. / ( 1 + hh * ( aid_zw + rID_zw ) );
    
    
    //Neutral Drug States
    const double co_D_O  = 1. / ( 1 + hh * ( bi_d + bb + rO_D ) );
    const double co_D_C1 = 1. / ( 1 + hh * ( aa + bin ) );
    const double co_D_C2 = 1. / ( 1 + hh * ( ain + be ) );
    const double co_D_C3 = 1. / ( 1 + hh * ( ae ) );
    const double co_D_I  = 1. / ( 1 + hh * ( ai_d + rI_D ) );
    
    double O_n , C1_n , C2_n , C3_n , I_n;
    double DO_n , DC1_n , DC2_n , DC3_n , DI_n;
    double D_O_n , D_C1_n , D_C2_n , D_C3_n , D_I_n;
    double Dzw_O_n , Dzw_C1_n , Dzw_C2_n , Dzw_C3_n , Dzw_I_n;
    
    
    int iter = 0;
    double err_sum = 1;
    while ( err_sum > 1E-100 && iter < 100 ) {
        
        O_n  = co_O * ( O_o + hh * ( aa * C1 + ai * I + rO_D * D_O + rOD * DO + rOD_zw *Dzw_O) );
        C1_n = co_C1 * ( C1_o + hh * ( ain * C2 + bb * O ) );
        C2_n = co_C2 * ( C2_o + hh * ( ae * C3 + bin * C1 ) );
        C3_n = co_C3 * ( C3_o + hh * ( be * C2 ) );
        I_n  = co_I * ( I_o + hh * ( bi * O + rI_D * D_I + rID * DI + rID_zw * Dzw_I) );
        
        //Charged Drug States
        DO_n  = co_DO * ( DO_o + hh * ( aa * DC1 + aid * DI + kOD  * O ) );
        DC1_n = co_DC1 * ( DC1_o + hh * ( ain * DC2 + bb * DO ) );
        DC2_n = co_DC2 * ( DC2_o + hh * ( ae * DC3 + bin * DC1 ) );
        DC3_n = co_DC3 * ( DC3_o + hh * ( be * DC2 ) );
        DI_n  = co_DI * ( DI_o + hh * ( bid * DO + kID  * I ) );
        
        //zwitterionic Drug States
        Dzw_O_n  = co_Dzw_O * ( Dzw_O_o + hh * ( aa * Dzw_C1 + aid_zw * Dzw_I + kOD_zw  * O ) );
        Dzw_C1_n = co_Dzw_C1 * ( Dzw_C1_o + hh * ( ain * Dzw_C2 + bb * Dzw_O ) );
        Dzw_C2_n = co_Dzw_C2 * ( Dzw_C2_o + hh * ( ae * Dzw_C3 + bin * Dzw_C1 ) );
        Dzw_C3_n = co_Dzw_C3 * ( Dzw_C3_o + hh * ( be * Dzw_C2 ) );
        Dzw_I_n  = co_Dzw_I * ( Dzw_I_o + hh * ( bid_zw * Dzw_O + kID_zw  * I ) );
        
        //Neutral Drug States
        D_O_n  = co_D_O * ( D_O_o + hh * ( aa * D_C1 + ai_d * D_I + kO_D  * O ) );
        D_C1_n = co_D_C1 * ( D_C1_o + hh * ( ain * D_C2 + bb * D_O ) );
        D_C2_n = co_D_C2 * ( D_C2_o + hh * ( ae * D_C3 + bin * D_C1 ) );
        D_C3_n = co_D_C3 * ( D_C3_o + hh * ( be * D_C2 ) );
        D_I_n  = co_D_I * ( D_I_o + hh * ( bi_d * D_O + kI_D  * I ) );
        
        
        
        err_sum = ( fabs( I - I_n ) + fabs( C3 - C3_n ) + fabs( C2 - C2_n ) + fabs( C1 - C1_n ) + fabs( O - O_n )
                   + fabs( DI - DI_n ) + fabs( DC3 - DC3_n) + fabs( DC2 - DC2_n ) + fabs( DC1 - DC1_n ) + fabs( DO - DO_n )
                   + fabs( D_I - D_I_n ) + fabs( D_C3 - D_C3_n) + fabs( D_C2 - D_C2_n ) + fabs( D_C1 - D_C1_n ) + fabs( D_O - D_O_n
                                                                                                                       + fabs( Dzw_I - Dzw_I_n ) + fabs( Dzw_C3 - Dzw_C3_n) + fabs( Dzw_C2 - Dzw_C2_n ) + fabs( Dzw_C1 - Dzw_C1_n ) + fabs( Dzw_O - Dzw_O_n )                                                                                                    ));
        
        
        
        
        
        I = I_n;
        C3 = C3_n;
        C2 = C2_n;
        C1 = C1_n;
        O = O_n;
        
        DI = DI_n;
        DC3 = DC3_n;
        DC2 = DC2_n;
        DC1 = DC1_n;
        DO = DO_n;
        
        Dzw_I = Dzw_I_n;
        Dzw_C3 = Dzw_C3_n;
        Dzw_C2 = Dzw_C2_n;
        Dzw_C1 = Dzw_C1_n;
        Dzw_O = Dzw_O_n;
        
        D_I = D_I_n;
        D_C3 = D_C3_n;
        D_C2 = D_C2_n;
        D_C1 = D_C1_n;
        D_O = D_O_n;
        
        
        iter++;
    }
    
    theCell->I_Kr = I_n;
    theCell->C3_Kr = C3_n;
    theCell->C2_Kr = C2_n;
    theCell->C1_Kr = C1_n;
    theCell->O_Kr = O_n;
    
    theCell->Ic_drug = DI_n;
    theCell->C3c_drug = DC3_n;
    theCell->C2c_drug = DC2_n;
    theCell->C1c_drug = DC1_n;
    theCell->Oc_drug = DO_n;
    
    theCell->In_drug = D_I_n;
    theCell->C3n_drug = D_C3_n;
    theCell->C2n_drug = D_C2_n;
    theCell->C1n_drug = D_C1_n;
    theCell->On_drug = D_O_n;
    
    theCell->zw_I_drug = Dzw_I_n;
    theCell->zw_C3_drug = Dzw_C3_n;
    theCell->zw_C2_drug = Dzw_C2_n;
    theCell->zw_C1_drug = Dzw_C1_n;
    theCell->zw_O_drug = Dzw_O_n;
    
    
    sum2=theCell->O_Kr+theCell->C1_Kr+theCell->C2_Kr+theCell->C3_Kr+theCell->I_Kr+ theCell->On_drug + theCell->C1n_drug + theCell->C2n_drug + theCell->C3n_drug + theCell->In_drug + theCell->Oc_drug + theCell->C1c_drug + theCell->C2c_drug + theCell->C3c_drug + theCell->Ic_drug + theCell->zw_I_drug + theCell->zw_C3_drug + theCell->zw_C2_drug + theCell->zw_C1_drug + theCell->zw_O_drug;

	double GKr = theCell->GKr;// * (1/(1+pow((Dofetilide/1.8),1)));;//0.024*1.7575;
	
    if (theCell->celltype==1)
	{
		GKr*=1.3;
	}
	if (theCell->celltype==2)
	{
		GKr*=0.8;
	}
	
	double IKr = GKr*sqrt(ko/5.4)*theCell->O_Kr*(v-EK);
	
	
	//%calculate IKs
	double xs1ss=1.0/(1.0+exp((-(v+11.60))/8.932));
	double txs1=817.3+1.0/(2.326e-4*exp((v+48.28)/17.80)+0.001292*exp((-(v+210.0))/230.0));
	//double dxs1=(xs1ss-xs1)/txs1;
	theCell->xs1 = xs1ss - (xs1ss - theCell->xs1)*exp(-dt/txs1);
	
	double xs2ss=xs1ss;
	double txs2=1.0/(0.01*exp((v-50.0)/20.0)+0.0193*exp((-(v+66.54))/31.0));
	//double dxs2=(xs2ss-xs2)/txs2;
	theCell->xs2 = xs2ss - (xs2ss - theCell->xs2)*exp(-dt/txs2);
	
	double KsCa=1.0+0.6/(1.0+ pow((3.8e-5/theCell->cai),1.4) );
	double GKs=0.0034;
	if( theCell->celltype==1 ) {
		GKs=GKs*1.4;
	}
	
	double IKs=GKs*KsCa*theCell->xs1*theCell->xs2*(v-EKs);
	
	// %calculate IK1
	double xk1ss=1.0/(1.0+exp(-(v+2.5538*ko+144.59)/(1.5692*ko+3.8115)));
	double txk1=122.2/(exp((-(v+127.2))/20.36)+exp((v+236.8)/69.33));
	//double dxk1=(xk1ss-xk1)/txk1;
	theCell->xk1 = xk1ss - (xk1ss - theCell->xk1)*exp(-dt/txk1);
	
	double rk1=1.0/(1.0+exp((v+105.8-2.6*ko)/9.493));
	double GK1=0.1908;
	if( theCell->celltype==1 ) {
		GK1=GK1*1.2;
	} else if( theCell->celltype==2 ) {
		GK1=GK1*1.3;
	}
	
	double IK1=GK1*sqrt(ko)*rk1*theCell->xk1*(v-EK);
	
	// %calculate INaCa_i
	double kna1=15.0;
	double kna2=5.0;
	double kna3=88.12;
	double kasymm=12.5;
	double wna=6.0e4;
	double wca=6.0e4;
	double wnaca=5.0e3;
	double kcaon=1.5e6;
	double kcaoff=5.0e3;
	double qna=0.5224;
	double qca=0.1670;
	double hca=exp((qca*v*F)/(R*T));
	double hna=exp((qna*v*F)/(R*T));
	double h1=1+theCell->nai/kna3*(1+hna);
	double h2=(theCell->nai*hna)/(kna3*h1);
	double h3=1.0/h1;
	double h4=1.0+theCell->nai/kna1*(1+theCell->nai/kna2);
	double h5=theCell->nai*theCell->nai/(h4*kna1*kna2);
	double h6=1.0/h4;
	double h7=1.0+nao/kna3*(1.0+1.0/hna);
	double h8=nao/(kna3*hna*h7);
	double h9=1.0/h7;
	double h10=kasymm+1.0+nao/kna1*(1.0+nao/kna2);
	double h11=nao*nao/(h10*kna1*kna2);
	double h12=1.0/h10;
	double k1=h12*cao*kcaon;
	double k2=kcaoff;
	double k3p=h9*wca;
	double k3pp=h8*wnaca;
	double k3=k3p+k3pp;
	double k4p=h3*wca/hca;
	double k4pp=h2*wnaca;
	double k4=k4p+k4pp;
	double k5=kcaoff;
	double k6=h6*theCell->cai*kcaon;
	double k7=h5*h2*wna;
	double k8=h8*h11*wna;
	double x1=k2*k4*(k7+k6)+k5*k7*(k2+k3);
	double x2=k1*k7*(k4+k5)+k4*k6*(k1+k8);
	double x3=k1*k3*(k7+k6)+k8*k6*(k2+k3);
	double x4=k2*k8*(k4+k5)+k3*k5*(k1+k8);
	double E1=x1/(x1+x2+x3+x4);
	double E2=x2/(x1+x2+x3+x4);
	double E3=x3/(x1+x2+x3+x4);
	double E4=x4/(x1+x2+x3+x4);
	double KmCaAct=150.0e-6;
	double allo=1.0/(1.0+ pow((KmCaAct/theCell->cai),2));
	double zna=1.0;
	double JncxNa=3.0*(E4*k7-E1*k8)+E3*k4pp-E2*k3pp;
	double JncxCa=E2*k2-E1*k1;
	double Gncx=0.0008;
	if( theCell->celltype==1 ) {
		Gncx=Gncx*1.1;
	} else if( theCell->celltype==2 ) {
		Gncx=Gncx*1.4;
	}
	double INaCa_i=0.8*Gncx*allo*(zna*JncxNa+zca*JncxCa);
	
	// %calculate INaCa_ss
	h1=1+theCell->nass/kna3*(1+hna);
	h2=(theCell->nass*hna)/(kna3*h1);
	h3=1.0/h1;
	h4=1.0+theCell->nass/kna1*(1+theCell->nass/kna2);
	h5=theCell->nass*theCell->nass/(h4*kna1*kna2);
	h6=1.0/h4;
	h7=1.0+nao/kna3*(1.0+1.0/hna);
	h8=nao/(kna3*hna*h7);
	h9=1.0/h7;
	h10=kasymm+1.0+nao/kna1*(1+nao/kna2);
	h11=nao*nao/(h10*kna1*kna2);
	h12=1.0/h10;
	k1=h12*cao*kcaon;
	k2=kcaoff;
	k3p=h9*wca;
	k3pp=h8*wnaca;
	k3=k3p+k3pp;
	k4p=h3*wca/hca;
	k4pp=h2*wnaca;
	k4=k4p+k4pp;
	k5=kcaoff;
	k6=h6*theCell->cass*kcaon;
	k7=h5*h2*wna;
	k8=h8*h11*wna;
	x1=k2*k4*(k7+k6)+k5*k7*(k2+k3);
	x2=k1*k7*(k4+k5)+k4*k6*(k1+k8);
	x3=k1*k3*(k7+k6)+k8*k6*(k2+k3);
	x4=k2*k8*(k4+k5)+k3*k5*(k1+k8);
	E1=x1/(x1+x2+x3+x4);
	E2=x2/(x1+x2+x3+x4);
	E3=x3/(x1+x2+x3+x4);
	E4=x4/(x1+x2+x3+x4);
	KmCaAct=150.0e-6;
	allo=1.0/(1.0+ pow((KmCaAct/theCell->cass),2));
	JncxNa=3.0*(E4*k7-E1*k8)+E3*k4pp-E2*k3pp;
	JncxCa=E2*k2-E1*k1;
	double INaCa_ss=0.2*Gncx*allo*(zna*JncxNa+zca*JncxCa);
	
	double k1p=949.5;
	double k1m=182.4;
	double k2p=687.2;
	double k2m=39.4;
	k3p=1899.0;
	double k3m=79300.0;
	k4p=639.0;
	double k4m=40.0;
	double Knai0=9.073;
	double Knao0=27.78;
	double delta=-0.1550;
	double Knai=Knai0*exp((delta*v*F)/(3.0*R*T));
	double Knao=Knao0*exp(((1.0-delta)*v*F)/(3.0*R*T));
	double Kki=0.5;
	double Kko=0.3582;
	double MgADP=0.05;
	double MgATP=9.8;
	double Kmgatp=1.698e-7;
	double H=1.0e-7;
	double eP=4.2;
	double Khp=1.698e-7;
	double Knap=224.0;
	double Kxkur=292.0;
	double P=eP/(1.0+H/Khp+theCell->nai/Knap+theCell->ki/Kxkur);
	double a1=(k1p * pow((theCell->nai/Knai),3) )/( pow((1.0+theCell->nai/Knai),3) + pow((1.0+theCell->ki/Kki),2) - 1.0);
	double b1=k1m*MgADP;
	double a2=k2p;
	double b2=(k2m * pow((nao/Knao),3) )/( pow((1.0+nao/Knao),3) + pow((1.0+ko/Kko),2) - 1.0);
	double a3=(k3p * pow((ko/Kko),2) )/( pow((1.0+nao/Knao),3) + pow((1.0+ko/Kko),2) - 1.0);
	double b3=(k3m*P*H)/(1.0+MgATP/Kmgatp);
	double a4=(k4p*MgATP/Kmgatp)/(1.0+MgATP/Kmgatp);
	double b4=(k4m * pow((theCell->ki/Kki),2) )/( pow((1.0+theCell->nai/Knai),3) + pow((1.0+theCell->ki/Kki),2) - 1.0);
	x1=a4*a1*a2+b2*b4*b3+a2*b4*b3+b3*a1*a2;
	x2=b2*b1*b4+a1*a2*a3+a3*b1*b4+a2*a3*b4;
	x3=a2*a3*a4+b3*b2*b1+b2*b1*a4+a3*a4*b1;
	x4=b4*b3*b2+a3*a4*a1+b2*a4*a1+b3*b2*a1;
	E1=x1/(x1+x2+x3+x4);
	E2=x2/(x1+x2+x3+x4);
	E3=x3/(x1+x2+x3+x4);
	E4=x4/(x1+x2+x3+x4);
	double zk=1.0;
	double JnakNa=3.0*(E1*a3-E2*b3);
	double JnakK=2.0*(E4*b1-E3*a1);
	double Pnak=30;
	if( theCell->celltype==1 ) {
		Pnak=Pnak*0.9;
	} else if( theCell->celltype==2 ) {
		Pnak=Pnak*0.7;
	}
	double INaK=Pnak*(zna*JnakNa+zk*JnakK);
	
	// %calculate IKb
	double xkb=1.0/(1.0+exp(-(v-14.48)/18.34));
	double GKb=0.003;
	if( theCell->celltype==1 ) {
		GKb=GKb*0.6;
	}
	double IKb=GKb*xkb*(v-EK);
	
	// %calculate INab
	double PNab=3.75e-10;
	double INab=PNab*vffrt*(theCell->nai*exp(vfrt)-nao)/(exp(vfrt)-1.0);
	
	// %calculate ICab
	double PCab=2.5e-8;
	double ICab=PCab*4.0*vffrt*(theCell->cai*exp(2.0*vfrt)-0.341*cao)/(exp(2.0*vfrt)-1.0);
	
	// %calculate IpCa
	double GpCa=0.0005;
	double IpCa=GpCa*theCell->cai/(0.0005+theCell->cai);
	
	//%%%%%%%%%%%%%%%%%%%%
	double I_total = INa+INaL+Ito+ICaL+ICaNa+ICaK+IKr+IKs+IK1+INaCa_i+INaCa_ss+INaK+INab+IKb+IpCa+ICab+I_inj;
	//%%%%%%%%%%%%%%%%%%%%%
	
	// %calculate diffusion fluxes
	double JdiffNa=(theCell->nass-theCell->nai)/2.0;
	double JdiffK=(theCell->kss-theCell->ki)/2.0;
	double Jdiff=(theCell->cass-theCell->cai)/0.2;
	
	// %calculate ryanodione receptor calcium induced calcium release from the jsr
	double bt=4.75;
	double a_rel=0.5*bt;
	double Jrel_inf=a_rel*(-ICaL)/(1.0 + pow((1.5/theCell->cajsr),8) );
	if( theCell->celltype==2 ){
		Jrel_inf=Jrel_inf*1.7;
	}
	double tau_rel=bt/(1.0+0.0123/theCell->cajsr);
	
	if( tau_rel<0.001 ) {
		tau_rel=0.001;
	}
	
	//double dJrelnp=(Jrel_inf-Jrelnp)/tau_rel;
	theCell->Jrelnp = Jrel_inf - (Jrel_inf - theCell->Jrelnp)*exp(-dt/tau_rel);
	
	double btp=1.25*bt;
	double a_relp=0.5*btp;
	double Jrel_infp=a_relp*(-ICaL)/(1.0 + pow((1.5/theCell->cajsr),8) );
	if( theCell->celltype==2 ) {
		Jrel_infp=Jrel_infp*1.7;
	}
	double tau_relp=btp/(1.0+0.0123/theCell->cajsr);
	
	if( tau_relp<0.001 ) {
		tau_relp=0.001;
	}
	
	//double dJrelp=(Jrel_infp-Jrelp)/tau_relp;
	theCell->Jrelp = Jrel_infp - (Jrel_infp - theCell->Jrelp)*exp(-dt/tau_relp);
	
	double fJrelp=(1.0/(1.0+KmCaMK/theCell->CaMKa));
	double Jrel = (1.0-fJrelp)*theCell->Jrelnp+fJrelp*theCell->Jrelp;
	// %calculate serca pump, ca uptake flux
	double Jupnp=0.004375*theCell->cai/(theCell->cai+0.00092);
	double Jupp=2.75*0.004375*theCell->cai/(theCell->cai+0.00092-0.00017);
	if( theCell->celltype==1 ) {
		Jupnp=Jupnp*1.3;
		Jupp=Jupp*1.3;
	}
	double fJupp=(1.0/(1.0+KmCaMK/theCell->CaMKa));
	double Jleak=0.0039375*theCell->cansr/15.0;
	double Jup=(1.0-fJupp)*Jupnp+fJupp*Jupp-Jleak;
	
	// %calculate tranlocation flux
	double Jtr=(theCell->cansr-theCell->cajsr)/100.0;
	
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	
	// %calcium buffer constants
	double cmdnmax=0.05;
	if( theCell->celltype==1 ) {
		cmdnmax=cmdnmax*1.3;
	}
	double kmcmdn=0.00238;
	double trpnmax=0.07;
	double kmtrpn=0.0005;
	double BSRmax=0.047;
	double KmBSR=0.00087;
	double BSLmax=1.124;
	double KmBSL=0.0087;
	double csqnmax=10.0;
	double kmcsqn=0.8;
	
	// %update intracellular concentrations, using buffers for cai, cass, cajsr
	double dnai=-(INa+INaL+3.0*INaCa_i+3.0*INaK+INab)*Acap/(F*vmyo)+JdiffNa*vss/vmyo;
	double dnass=-(ICaNa+3.0*INaCa_ss)*Acap/(F*vss)-JdiffNa;
	theCell->nai = theCell->nai + dt*dnai;
	theCell->nass = theCell->nass + dt*dnass;
	
	double dki=-(Ito+IKr+IKs+IK1+IKb+I_inj-2.0*INaK)*Acap/(F*vmyo)+JdiffK*vss/vmyo;
	double dkss=-(ICaK)*Acap/(F*vss)-JdiffK;
	theCell->ki = theCell->ki + dt*dki;
	theCell->kss = theCell->kss + dt*dkss;
	
	double Bcai=1.0/(1.0 + cmdnmax * kmcmdn / pow((kmcmdn+theCell->cai),2) + trpnmax * kmtrpn / pow((kmtrpn+theCell->cai),2) );
	double dcai=Bcai*(-(IpCa+ICab-2.0*INaCa_i)*Acap/(2.0*F*vmyo)-Jup*vnsr/vmyo+Jdiff*vss/vmyo);
	theCell->cai = theCell->cai + dt*dcai;
	double Bcass=1.0/(1.0 + BSRmax * KmBSR / pow((KmBSR+theCell->cass),2) + BSLmax * KmBSL / pow((KmBSL+theCell->cass),2) );
	double dcass=Bcass*(-(ICaL-2.0*INaCa_ss)*Acap/(2.0*F*vss)+Jrel*vjsr/vss-Jdiff);
	theCell->cass = theCell->cass + dt*dcass;
	
	double dcansr=Jup-Jtr*vjsr/vnsr;
	theCell->cansr = theCell->cansr + dt*dcansr;
	
	double Bcajsr=1.0/(1.0 + csqnmax * kmcsqn / pow((kmcsqn+theCell->cajsr),2) );
	double dcajsr=Bcajsr*(Jtr-Jrel);
	theCell->cajsr = theCell->cajsr + dt*dcajsr;
	
	
	
	return I_total;
}

