//ecc over y2=x3-x+1
#include<iostream>
#include<random>
using namespace std;
int a=1,b=6,p=17;

struct point
{
    int x=0;
    int y=0;
};

void Bob(point *cip,int len,point B);

point G,PA,PB;

void printpoint(point a)
{
    cout<<"( "<<a.x<<","<<a.y<<" )";
}

int mod_inv(int a, int n)
{
    int m0 = n;
    int y0 = 0,y1 = 1;
    if (n == 1)
        return 0;
    a=a%n;
    if(a<0) 
        a=a+n;
    while (a > 1) {
        int quot = a / n;
        int t = n;
        n = a % n;
        a = t; 
        t = y0; 
        y0=y1-y0*quot;      
        y1= t;      
    }
    if (y1 < 0) 
        y1 += m0;
    return y1;
}

point addition(point q,point r)
{
    point result;
    int m=0;
   
    int num=r.y-q.y;
    int den=r.x-q.x;
    
    if(den<0)
    {
       num*=-1;
       den*=-1;
    }
    
    int temp=mod_inv(den,p);
    m=(num*temp)%p;
 
    if (m<0)
        m+=p;
    
    result.x=(m*m-q.x-r.x)%p;
    
    if(result.x<0)
        result.x+=p;
    
    result.y=(m*(q.x-result.x)-q.y)%p;
    
    if(result.y<0)
        result.y+=p;
        
    return result;
}
point doubling(point q)
{
    point result;
    int m=0;
    
    int o=mod_inv(2*q.y,p);

    m=((3*q.x*q.x+a)*o)%p;
    
    result.x=(m*m-q.x-q.x)%p;
    
    if(result.x<0)
        result.x+=p;
    
    result.y=(m*(q.x-result.x)-q.y)%p;
    
    if(result.y<0)
        result.y+=p;
    
    return result;
}
point kG(point P,int d)
{                       
    if (d == 1) 
        return P;
    else if (d%2==1)
        return addition(P,kG(P,d-1)); 
    else
        return kG(doubling(P),d/2);   
}
void encrypt(point B,int *imsg,int len,point *cip) //cipher text = [k*B, Pm+k*Pb]
{
    point Pm;

    for (int i = 0; i < len*2; i+=2)
    {
        Pm.x=imsg[i];
        Pm.y=imsg[i+1];
        
        int l=p-1;
        int k=1+(rand()%l);
        
        cip[i]=kG(B,k);
        
        point temp=kG(PB,k);
        cip[i+1]=addition(Pm,temp);
    
    }
    cout<<"\n\nCipherText Points :\t";
    for(int i=0;i<len*2;i++)
    {
        if(i==0)
            cout<<"{ [ ";
        
        if(i%2==0 && i!=0)
            cout<<" ] , [ ";
        
        printpoint(cip[i]);
        
         if(i==len*2-1)
            cout<<" ] }";
    }
    cout<<"\n\n";
    
}

void decrypt(int pvt,point *cip,int len)//Pm + kPb - b(kG) = Pm (decrypted)
{
    string decrypted;
    cout<<"\n\nDecrypted Points :\t";

    for(int i=0;i<len;i+=2)
    {
        point temp=kG(cip[i],pvt);
        
        temp.y=((-1)*temp.y)%p;
        if(temp.y<0)
            temp.y+=p;
        
        point temp1=addition(temp,cip[i+1]);
        
        printpoint(temp1);
        
        decrypted+=(temp1.x+65-1);
        decrypted+=(temp1.y+65-1);
    }

    cout<<"\n\nDecrypted PlainText :\t"<<decrypted;
    cout<<"\n\n";
}

void Alice(int *imsg,int len,point B)
{
    
    cout<<"\n\n===========/ ALICE /============\n";
    point cip[len*2];
    cout<<"\nPrivate Key :\t"<<a;
    cout<<"\nPublic Key :\t";
    
    printpoint(PA);
    
    encrypt(B,imsg,len,cip);
    
    Bob(cip,len,G);
}

void Bob(point *cip,int len,point B)
{
    
    cout<<"\n===========/ BOB /============\n";
    cout<<"\nPrivate Key :\t"<<b;
    cout<<"\nPublic Key :\t";
    
    printpoint(PB);
    
    decrypt(b,cip,len);
}

int main()
{
    G.x=15;
    G.y=13;
    cout<<"\n==================================\n";
    cout<<"\nBase Point :\t";
    printpoint(G);
    
    string msg;
    cout<<"\nEnter Plaintext :\t";
    cin>>msg;

    int imsg[msg.length()];
    
    for(int i=0;i<msg.length();i++)
    {
        imsg[i]=((int)msg[i]-65)+1;
    }
    
    PA=kG(G,a);
    
    PB=kG(G,b);
    
    Alice(imsg,msg.length(),G);

}