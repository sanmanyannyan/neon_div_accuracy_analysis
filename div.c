#include <arm_neon.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <locale.h>
#include <math.h>

union f32_u32 {
	float32_t f;
	uint32_t i;
};

void print_float_bits(float32_t flt){
	union f32_u32 un;
	un.f = flt;
	uint32_t u = un.i;
	uint32_t i,j = 31;
	//printf("u = %u\n", u);
	for(i = (1u << 31) ; i!=0 ; i >>= 1, --j){
		if(i & u){
			printf("1");
		} else {
			printf("0");
		}
		switch(j){
		case 31:
		case 23:
			printf(" ");
			break;
		case 19:
		case 15:
		case 11:
		case 7:
		case 3:
			printf("'");
			break;
		}
	}
	printf("\n");
}

float32x4_t div0(float32x4_t a, float32x4_t b){
	float32x4_t c;
	c = vrecpeq_f32(b);
	return vmulq_f32(a, c);
}

int main(int argc,char *argv[]){
	//floatx4_t vx, vy, vz;
	const int vector_length = 4;
	float32x4_t vx, vy;
	float32_t *x, *y;
	float32_t *destE,*destO;
	int maxIdx = 0;
	double max_reldif = 0;
	double reldif;
	float32_t true_val,est_val;
	const int n = 1048576;
	x = malloc(sizeof(float32_t) * n);
	y = malloc(sizeof(float32_t) * n);
	destE = malloc(sizeof(float32_t) * n);
	destO = malloc(sizeof(float32_t) * n);
	for(int i=0 ; i<n ; ++i){
		x[i] = 1.f;
		y[i] = (float32_t)i + 2.f;
	}
	for(int i=0 ; i<n ; i+= vector_length){
		float32x4_t vx = vld1q_f32(x + i);
		float32x4_t vy = vld1q_f32(y + i);
		float32x4_t vz = div0(vx, vy);
		vst1q_f32(destE + i, vz);
		destO[i + 0] = x[i + 0] / y[i + 0]; 
		destO[i + 1] = x[i + 1] / y[i + 1]; 
		destO[i + 2] = x[i + 2] / y[i + 2]; 
		destO[i + 3] = x[i + 3] / y[i + 3]; 
		
		reldif = fabs((destE[i + 0] - destO[i + 0]) / destO[i + 0]);
		if( reldif > max_reldif ){
			max_reldif = reldif;
			true_val = destO[i + 0];
			est_val = destE[i + 0];
			maxIdx = i;
		}
		reldif = fabs((destE[i + 1] - destO[i + 1]) / destO[i + 1]);
		if( reldif > max_reldif ){
			max_reldif = reldif;
			true_val = destO[i + 1];
			est_val = destE[i + 1];
			maxIdx = i;
		}
		reldif = fabs((destE[i + 2] - destO[i + 2]) / destO[i + 2]);
		if( reldif > max_reldif ){
			max_reldif = reldif;
			true_val = destO[i + 2];
			est_val = destE[i + 2];
			maxIdx = i;
		}
		reldif = fabs((destE[i + 3] - destO[i + 3]) / destO[i + 3]);
		if( reldif > max_reldif ){
			max_reldif = reldif;
			true_val = destO[i + 3];
			est_val = destE[i + 3];
			maxIdx = i;
		}
	}
	printf("max relative difference:%f\n",max_reldif);
	printf("true value:%f = 1 / %f\n", true_val, y[maxIdx]);
	printf(" est value:%f\n", est_val);
	printf("true value:");
	print_float_bits(true_val);
	printf(" est value:");
	print_float_bits(est_val);
	return 0;
}

