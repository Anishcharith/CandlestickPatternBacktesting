#include <fstream>
#include <sstream>
#include<cstring>
#include<iostream>

char temp[10];
int tot_row=0;

using namespace std;

/*------------------------------------------------------------------------------------------------*/

class candle
{
	public:
	char date[10];
	float open;
	float close;
	float high;
	float low;
	int candleno;	
};

class chart
{
	char name[10];
	char timeframe[20];
	
	int n_candle;
	int n_patterns;
	public:
	candle* getData(char *FILE);
	candle candlestick[1000];
}company[100];

candle* chart::getData(char *FILE)
{
 
	std::ifstream file(FILE);
	
    for(int row = 1; row < 1000; ++row)
    {
        std::string line;
        std::getline(file, line);
        if ( !file.good() )
	{
		tot_row=row;
        	break;
	}

        std::stringstream iss(line);

        for (int col = 0; col < 5; ++col)
        {
            std::string val;
            std::getline(iss, val, ',');
            if ( !iss.good() )
                break;

            std::stringstream convertor(val);
		if(col==0)
            	convertor >> candlestick[row].date;
		if(col==1)
		{
            	convertor >> candlestick[row].open;
		}
		if(col==2)
            	convertor >> candlestick[row].high;
		if(col==3)
            	convertor >> candlestick[row].low;
		if(col==4)
            	convertor >> candlestick[row].close;
			candlestick[row].candleno=col;	
        }
    }

	return  candlestick;	
};

/*------------------------------------------------------------------------------------------------*/

class pattern
{
	int NoOfCandles;
	int Avg[1000];
	int* Valid;	
	
public:
	pattern()
	{
		Valid=new int[1000];
		for(int i=0;i<1000;i++)
		{
			Valid[i]=0;
		}
	}
	
	void mvgAvg(chart B)
	{
		int sum;
		for(int i=2;i<tot_row;i++)
		{
			sum=0;
			for(int j=0;j<9;j++)
			{	
				sum+=B.candlestick[j].close;
			}
			Avg[i]=sum/9;
		}
	}
	
	int* Bullmarubozu(chart B)
	{	
		mvgAvg(B);
		NoOfCandles = 1;
		for(int i=11;i<tot_row;i++)
		{
			
				
				if((B.candlestick[i].close>=(.95)*B.candlestick[i].high)&&(B.candlestick[i].open<=(1.05)*B.candlestick[i].low))
					Valid[i]=1;
			
		}
		return Valid;
	
	}
	int* Doji(chart B)
	{
		NoOfCandles=1;
		for(int i=11;i<tot_row;i++)
		{	
			if(((B.candlestick[i].close-B.candlestick[i].open)<=.01*B.candlestick[i].open)&&((B.candlestick[i].close-B.candlestick[i].open)>=-.01*B.candlestick[i].open))
				Valid[i]=1;
		}
		return Valid;
	}
	int* Hammer(chart B)
	{
		NoOfCandles=1;
		for(int i=11;i<tot_row;i++)
		{
			if((B.candlestick[i].close>=.99*B.candlestick[i].high)&&((B.candlestick[i].open-B.candlestick[i].low)>=.01*B.candlestick[i].open))
				Valid[i]=1;
		}
		return Valid;	
	}
	int* Harami(chart B)
	{
		NoOfCandles = 2;
		for(int i=11;i<tot_row-1;i++)
		{
			if((B.candlestick[i].close<B.candlestick[i+1].open)&&(B.candlestick[i].open>=B.candlestick[i+1].close)&&(B.candlestick[i+1].close>B.candlestick[i+1].open)&&(B.candlestick[i].close<B.candlestick[i].open))
				Valid[i+1]=1;
		}
		return Valid;
	}
};

/*------------------------------------------------------------------------------------------------*/

class strategy
{

	float stoploss;
	float target;
	float buy;
	float sell;
	int profit[1000];
	int loss[1000];
	int day;
	int profitcount;
	int losscount;

public:
	strategy()
	{
		profitcount=0;
		losscount=0;
	}
	float calculate(chart B)
	{	int* Valid;
		int choice;
		pattern pat;
		day=0;
		profitcount=0;
		losscount=0;
		cout<<"Choose Pattern \n 1. Bullish Marubozu \n 2. Doji \n 3. Hammer \n 4. Harami \n";
		cin>>choice;
		switch(choice)
		{
			case 1:	
				Valid=pat.Bullmarubozu(company[0]);
				break;
			case 2:	
				Valid=pat.Doji(company[0]);
				break;
			case 3:	
				Valid=pat.Hammer(company[0]);
				break;
			case 4:	
				Valid=pat.Harami(company[0]);
				break;
			default :
				cout<<"Invalid choice \n";
				return 0;
		}
		cout<<"Enter the Target and Stoploss \n";
		cin>>target;
		cin>>stoploss;
		
		for(int i=0;i<tot_row;i++)
		{
			if(Valid[i]==1)
			{	
				buy=B.candlestick[i].close;
				cout<<"\nPattern comfirmed"<<endl<<"Bought at : "<<B.candlestick[i].date<<",Price : "<<buy<<endl;
				for(;i<tot_row;i++)
				{
					if(B.candlestick[i].close>=(1+target/100)*buy)
					{
						profit[i]=1;
						profitcount++;
						cout<<"Target Hit"<<endl<<"Sold at   : "<<B.candlestick[i].date<<",Price : "<<buy*(1+target/100)<<endl;
						break;
					}
					else if(B.candlestick[i].close<=(1-stoploss/100)*buy)
					{
						loss[i]=1;
						losscount++;
						cout<<"Stoploss Breached"<<endl<<"Sold at   : "<<B.candlestick[i].date<<",Price : "<<buy*(1-stoploss/100)<<endl;
						break;
					}
					else
						day++;
				}
			}

		}
		cout<<"Total profit = "<<profitcount*target<<"%"<<endl;
		cout<<"Total loss   = "<<losscount*stoploss<<"%"<<endl;
		return 	profitcount*target-losscount*stoploss;
	}
	
	void compare(chart B)
	{
		float profit1,profit2;
		int *Valid1;
		int *Valid2;
		cout<<"\n\n\nFirst Strategy \n";
		profit1=calculate(B);
		cout<<"\n\n\nSecond Strategy \n";
		profit2=calculate(B);
		cout<<"\n\nFirst  strategy - "<<profit1<<"%"<<endl;
		cout<<"Second Strategy - "<<profit2<<"%"<<endl;
	}
	
	void compcal(chart A, chart B)
	{
		float profit1,profit2;
		int *Valid1;
		int *Valid2;
		cout<<"\n\n\nFirst Company \n";
		profit1=calculate(A);
		cout<<"\n\n\nSecond Company \n";
		profit2=calculate(B);
		cout<<"\n\nFirst  Company - "<<profit1<<"%"<<endl;
		cout<<"Second Company - "<<profit2<<"%"<<endl;
	}
};

/*------------------------------------------------------------------------------------------------*/

int main()
{	
	int comp,comp1,comp2,choice;
	char Companynames[10][10];
	int NoOfComp=5;
	strategy strat; 
	company[0].getData("blah.csv");
	company[1].getData("data.csv");
	company[2].getData("Bharti.csv");
	company[3].getData("IDEA.csv");
	company[4].getData("YESBANK.csv");
	cout<<"Enter your choice \n 1. Backtest strategy \n 2. Compare 2 strategies \n 3. Compare Companies \n 4. Display historic data \n";
	cin>>choice;
	switch(choice)
	{
		case 1:	
			cout<<"Enter Company \n 1 - Asianpaints \n 2 - Indraprastha Gas Limited \n 3 - Bharti Airtel \n 4 - Idea \n 5 - Yes Bank \n";
			cin>>comp;
			strat.calculate(company[comp]);
			break;
		case 2:
			cout<<"Enter Company \n 1 - Asianpaints \n 2 - Indraprastha Gas Limited \n 3 - Bharti Airtel \n 4 - Idea \n 5 - Yes Bank \n";
			cin>>comp;
			strat.calculate(company[comp]);
			break;
		case 3:
			cout<<"Enter first company \n 1 - Asianpaints \n 2 - Indraprastha Gas Limited \n 3 - Bharti Airtel \n 4 - Idea \n 5 - Yes Bank \n";
			cin>>comp1;
			cout<<"\n\nEnter Second company \n 1 - Asianpaints \n 2 - Indraprastha Gas Limited \n 3 - Bharti Airtel \n 4 - Idea \n 5 - Yes Bank \n";
			cin>>comp2;
			strat.compcal(company[comp1],company[comp2]);
			break;
		default:
			cout<<"Invalid data \n";
	}
}		

