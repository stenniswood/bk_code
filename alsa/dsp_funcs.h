#ifndef _DSP_FUNCS_H_
#define _DSP_FUNCS_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long t_index;
typedef double t_real;
typedef long t_signed;
//extern clock_t  MFCCTime;


t_index Order_Log2( t_index num );

void xft 	( t_real* vr, t_real* vi, t_signed order, t_signed s );

void Magnitude_And_Energy_Of_Real_Vectors( 
								t_real* v_mag, t_real* v_imm,
								t_index size, t_real &energy );								

void mfcc	( t_real  *dp_fft,  t_real  *dp_mfcc, t_real d_sam,
		   	  t_signed l_nfft, t_signed l_nmfcc );


		   
#ifdef __cplusplus
}
#endif

#endif