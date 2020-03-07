#include "stdafx.h"
#define MAX_LEN_LINE 10
#define STEP 1000


VOID LineRegression(INT iCount, DOUBLE* mas, DOUBLE* pA, DOUBLE* pB)
{
	DOUBLE xi = 0.0;
	DOUBLE xxi = 0.0;

	DOUBLE yi = 0.0;

	DOUBLE xiyi = 0.0;

	for (int i = 0; i < iCount; i++)
	{
		DOUBLE x = DOUBLE(i + 1);
		xi += x;
		xxi += x * x;
		yi += mas[i];
		xiyi += x * mas[i];
	}

	(*pA) = (xi * yi - iCount * xiyi) / (xi * xi - iCount * xxi);
	(*pB) = (xi * xiyi - xxi * yi) / (xi * xi - iCount * xxi);
}

VOID SquareRegression(INT iCount, DOUBLE* mas, DOUBLE* pA, DOUBLE* pB, DOUBLE* pC)
{
	DOUBLE xi = 0.0;
	DOUBLE xxi = 0.0;
	DOUBLE xxxi = 0.0;
	DOUBLE xxxxi = 0.0;

	DOUBLE yi = 0.0;

	DOUBLE xiyi = 0.0;
	DOUBLE xxiyi = 0.0;

	for (int i = 0; i < iCount; i++)
	{
		DOUBLE x = DOUBLE(i + 1);
		DOUBLE y = mas[i];
		xi += x;
		xxi += x * x;
		xxxi += x * x * x;
		xxxxi += x * x * x * x;
		yi += y;
		xiyi += x * y;
		xxiyi += x * x * y;
	}

	DOUBLE a[3] = { xxi,xxxi,xxxxi };
	DOUBLE b[3] = { xi,xxi,xxxi };
	DOUBLE c[3] = { (DOUBLE)iCount,xi,xxi };
	DOUBLE vec[3] = { yi,xiyi,xxiyi };

	DOUBLE tmp = 0.0;
	//убираем a1 в 0
	tmp = a[1] / a[0];

	a[1] -= a[0] * tmp;
	b[1] -= b[0] * tmp;
	c[1] -= c[0] * tmp;
	vec[1] -= vec[0] * tmp;

	//убираем a2 в 0
	tmp = a[2] / a[0];

	a[2] -= a[0] * tmp;
	b[2] -= b[0] * tmp;
	c[2] -= c[0] * tmp;
	vec[2] -= vec[0] * tmp;

	//убираем b2 в 0
	tmp = b[2] / b[1];

	b[2] -= b[1] * tmp;
	c[2] -= c[1] * tmp;
	vec[2] -= vec[1] * tmp;

	/*(*pC)= vec[2] / c[2];
	(*pB)*/
	//убираем c1 в 0
	tmp = c[1] / c[2];

	c[1] -= c[2] * tmp;
	vec[1] -= vec[2] * tmp;

	//убираем c0 в 0
	tmp = c[0] / c[2];

	c[0] -= c[2] * tmp;
	vec[0] -= vec[2] * tmp;

	//убираем b0 в 0
	tmp = b[0] / b[1];

	b[0] -= b[1] * tmp;
	vec[0] -= vec[1] * tmp;

	(*pA) = vec[0] / a[0];
	(*pB) = vec[1] / b[1];
	(*pC) = vec[2] / c[2];
}

DOUBLE CalcDif(INT iCount,
	DOUBLE ALine, DOUBLE BLine,
	DOUBLE ASquare, DOUBLE BSquare, DOUBLE CSquare)
{
	DOUBLE* res = new DOUBLE[iCount];
	DOUBLE avg = 0.0;
	for (int i = 0; i < iCount; i++)
	{
		DOUBLE x = DOUBLE(i + 1);

		DOUBLE yLine = ALine * x + BLine;
		DOUBLE ySquare = ASquare * x * x + BSquare * x + CSquare;
		res[i] = fabs(fabs(yLine) - fabs(ySquare));

		avg += res[i];
	}

	//посчет среднего
	avg /= (DOUBLE)(iCount);

	DOUBLE displine = 0.0;
	for (int i = 0; i < iCount; i++)
	{
		displine += pow(avg - res[i], 2.0);
	}
	displine = sqrt(displine / ((DOUBLE)iCount));

	delete[] res;


	return displine;
}

DOUBLE calc(INT iCount, DOUBLE* input)
{
	DOUBLE aLine;
	DOUBLE bLine;
	DOUBLE aSquare;
	DOUBLE bSquare;
	DOUBLE cSquare;

	LineRegression(iCount, input, &aLine, &bLine);
	SquareRegression(iCount, input, &aSquare, &bSquare, &cSquare);

	return CalcDif(iCount, aLine, bLine, aSquare, bSquare, cSquare);
}

int main()
{
	double res = 1.0;

	DOUBLE input[MAX_LEN_LINE];
	
	INT la[5] = { MAX_LEN_LINE, 4,3,2, 1 };
	CNeuro* Neuro = new CNeuro(CFunction::Bent_identity, CFunction::Bent_identityDx, 0.005f, TRUE, 5, la);
	
	srand((UINT)time(0));

	

	int ic = 1;
	int icst= 0;
	int icend = 0;
	while (true)
	{
		ic++;
		for (int j = 0; j < MAX_LEN_LINE; j++)
			input[j] = (double)(rand() % 10000) / 1000.0;
		DOUBLE output = *Neuro->CalcNeuro(MAX_LEN_LINE, input);
		DOUBLE ans = calc(MAX_LEN_LINE, input);
		Neuro->CalcNeuroError(1, &(ans));
		DOUBLE erall = Neuro->GetError();
		DOUBLE era = Neuro->GetErrorLast();
		Neuro->CalcNeuroCorrect();

		
		if ((ic % STEP) == 0)
		{
			
				

			
			if (erall > 0.0001)
			{
				icst = ic / STEP;
				
				printf("\n%5d %20.10f %20.10f (m=%20.10f n=%20.10f) %d",
					ic / STEP, erall, era, ans, output, icst- icend);

				icend = icst;
			}
		}
	}

	return 0;
}