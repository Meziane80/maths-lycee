#include <eadk.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "Maths Lycee";
const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level"))) = 0;

#define BG 0xFFFF
#define NAVY 0x10A3
#define ORANGE 0xFD20
#define GREY 0xCE79
#define BLACK 0x0000
#define GREEN 0x2646
#define RED 0xE8A4

static void rect(int x,int y,int w,int h,uint16_t c){
  eadk_display_push_rect_uniform((eadk_rect_t){x,y,w,h},c);
}
static void text(int x,int y,const char *s,uint16_t fg,uint16_t bg){
  eadk_display_draw_string(s,(eadk_point_t){x,y},false,fg,bg);
}
static void frame(const char *title){
  rect(0,0,320,240,BG); rect(0,0,320,27,NAVY);
  text(8,6,title,BG,NAVY);
  text(8,224,"HAUT/BAS  EXE:OK  BACK:RETOUR",NAVY,BG);
}
static int pressed(eadk_key_t k){
  eadk_keyboard_state_t s=eadk_keyboard_scan();
  return eadk_keyboard_key_down(s,k);
}
static void wait_release(void){
  while(pressed(eadk_key_up)||pressed(eadk_key_down)||pressed(eadk_key_left)||
        pressed(eadk_key_right)||pressed(eadk_key_ok)||pressed(eadk_key_back)||
        pressed(eadk_key_exe)) eadk_timing_msleep(25);
}
static eadk_key_t wait_key(void){
  for(;;){
    eadk_keyboard_state_t s=eadk_keyboard_scan();
    eadk_key_t keys[]={eadk_key_up,eadk_key_down,eadk_key_left,eadk_key_right,
                       eadk_key_ok,eadk_key_back,eadk_key_exe};
    for(unsigned i=0;i<sizeof(keys)/sizeof(keys[0]);i++){
      if(eadk_keyboard_key_down(s,keys[i])){
        eadk_timing_msleep(140); wait_release(); return keys[i];
      }
    }
    eadk_timing_msleep(20);
  }
}
static void page(const char *title,const char *lines[],int n){
  int p=0, maxp=(n+7)/8;
  if(maxp<1)maxp=1;
  for(;;){
    frame(title);
    char counter[20]; snprintf(counter,sizeof(counter),"%d/%d",p+1,maxp);
    text(285,6,counter,BG,NAVY);
    for(int i=0;i<8 && p*8+i<n;i++) text(8,39+i*21,lines[p*8+i],BLACK,BG);
    text(8,204,"GAUCHE/DROITE: PAGES",ORANGE,BG);
    eadk_key_t k=wait_key();
    if(k==eadk_key_right && p<maxp-1)p++;
    else if(k==eadk_key_left && p>0)p--;
    else if(k==eadk_key_ok||k==eadk_key_exe||k==eadk_key_back)return;
  }
}
static int menu(const char *title,const char *items[],int count){
  int pos=0;
  for(;;){
    frame(title);
    text(8,31,"Selectionne une rubrique :",GREY,BG);
    for(int i=0;i<count;i++){
      int y=55+i*23;
      if(i==pos){rect(5,y-3,310,20,0xFFDF);text(11,y,items[i],ORANGE,0xFFDF);}
      else text(11,y,items[i],BLACK,BG);
    }
    eadk_key_t k=wait_key();
    if(k==eadk_key_up)pos=(pos+count-1)%count;
    else if(k==eadk_key_down)pos=(pos+1)%count;
    else if(k==eadk_key_ok||k==eadk_key_exe)return pos;
    else if(k==eadk_key_back)return -1;
  }
}
static int digit(eadk_key_t k){
  if(k==eadk_key_zero)return 0;
  if(k==eadk_key_one)return 1;
  if(k==eadk_key_two)return 2;
  if(k==eadk_key_three)return 3;
  if(k==eadk_key_four)return 4;
  if(k==eadk_key_five)return 5;
  if(k==eadk_key_six)return 6;
  if(k==eadk_key_seven)return 7;
  if(k==eadk_key_eight)return 8;
  if(k==eadk_key_nine)return 9;
  return -1;
}
static double number(const char *label){
  char buf[24]=""; int len=0, dot=0, neg=0;
  for(;;){
    frame("SAISIE D'UN NOMBRE");
    text(8,48,label,BLACK,BG); rect(8,83,304,30,0xEF7D);
    text(15,91,buf, NAVY,0xEF7D);
    text(8,135,"Touches numeriques pour saisir",GREY,BG);
    text(8,155,". : virgule   +/- : signe",GREY,BG);
    text(8,177,"EXE : valider   BACK : effacer",GREY,BG);
    eadk_key_t k=wait_key();
    int d=digit(k);
    if(d>=0 && len<20){buf[len++]=(char)('0'+d);buf[len]=0;}
    else if(k==eadk_key_dot && !dot && len<20){buf[len++]='.';buf[len]=0;dot=1;}
    else if(k==eadk_key_minus && len==0){buf[len++]='-';buf[len]=0;neg=1;}
    else if(k==eadk_key_back){if(len){if(buf[len-1]=='.')dot=0;if(buf[len-1]=='-')neg=0;buf[--len]=0;}else return NAN;}
    else if(k==eadk_key_ok||k==eadk_key_exe){if(len && strcmp(buf,"-")!=0)return atof(buf);}
  }
}
static int integer(const char *label){double x=number(label);return isnan(x)?-1:(int)x;}
static void result1(const char *title,const char *a){
  const char *l[]={a}; page(title,l,1);
}
static void fractions(void){
  const char *opts[]={"Simplifier","Addition","Soustraction","Multiplication","Division","Comparer","Decimal"};
  for(;;){
    int c=menu("FRACTIONS",opts,7); if(c<0)return;
    double an=number("Numerateur A"), ad=number("Denominateur A");
    if(isnan(an)||isnan(ad))continue;
    if(ad==0){result1("ERREUR","Denominateur nul");continue;}
    double bn=0,bd=1;
    if(c>0 && c<6){
      bn=number("Numerateur B");bd=number("Denominateur B");
      if(isnan(bn)||isnan(bd))continue;
      if(bd==0){result1("ERREUR","Denominateur nul");continue;}
    }
    double n=an,d=ad; char line[48],line2[48];
    if(c==1){n=an*bd+bn*ad;d=ad*bd;}
    if(c==2){n=an*bd-bn*ad;d=ad*bd;}
    if(c==3){n=an*bn;d=ad*bd;}
    if(c==4){if(bn==0){result1("ERREUR","Division par zero");continue;}n=an*bd;d=ad*bn;}
    if(c==5){
      double diff=an*bd-bn*ad;
      snprintf(line,sizeof(line),"%s",diff<0?"A < B":diff>0?"A > B":"A = B");
      result1("COMPARAISON",line);continue;
    }
    if(d<0){n=-n;d=-d;}
    long nn=(long)llround(n),dd=(long)llround(d),g=1;
    long aa=labs(nn),bb=labs(dd);
    while(bb){long t=aa%bb;aa=bb;bb=t;} if(aa)g=aa;
    nn/=g;dd/=g;
    snprintf(line,sizeof(line),"Fraction : %ld / %ld",nn,dd);
    snprintf(line2,sizeof(line2),"Decimal : %.6g",(double)nn/dd);
    const char *lines[]={line,line2};page("RESULTAT",lines,2);
  }
}
static void percentages(void){
  const char *opts[]={"p % de N","A en % de B","Taux evolution","Augmentation","Diminution","Coeff. multiplicateur","Valeur initiale"};
  for(;;){
    int c=menu("POURCENTAGES",opts,7);if(c<0)return;
    double a=number(c==0?"Pourcentage p":c==1?"Partie A":c==2||c==6?"Valeur initiale":"Valeur de depart");
    if(isnan(a))continue;
    double b=number(c==0?"Valeur N":c==1?"Total B":c==2||c==6?"Valeur finale":"Taux en %");
    if(isnan(b))continue;
    char l1[48],l2[48];double v=0;
    if(c==0)v=a*b/100;
    else if(c==1){if(b==0){result1("ERREUR","Total nul");continue;}v=a/b*100;}
    else if(c==2){if(a==0){result1("ERREUR","Valeur initiale nulle");continue;}v=(b-a)/a*100;}
    else if(c==3)v=a*(1+b/100);
    else if(c==4)v=a*(1-b/100);
    else if(c==5)v=1+b/100;
    else {double cm=1+b/100;if(cm==0){result1("ERREUR","Coefficient nul");continue;}v=a/cm;}
    snprintf(l1,sizeof(l1),"Resultat : %.7g",v);
    if(c==1||c==2)strcat(l1," %");
    snprintf(l2,sizeof(l2),"Formule : %s",c==0?"N x p / 100":c==1?"A / B x 100":c==2?"(VF-VI) / VI x 100":c==3?"VI x (1+t)":c==4?"VI x (1-t)":c==5?"1+t":"VF / CM");
    const char *ls[]={l2,l1};page("POURCENTAGE",ls,2);
  }
}
static void nuage(void){
  int n=integer("Nombre de points (2 a 12)");
  if(n<2||n>12)return;
  double x[12],y[12],sx=0,sy=0;
  for(int i=0;i<n;i++){
    char label[32];snprintf(label,sizeof(label),"Point %d : x",i+1);x[i]=number(label);
    snprintf(label,sizeof(label),"Point %d : y",i+1);y[i]=number(label);
    if(isnan(x[i])||isnan(y[i]))return;sx+=x[i];sy+=y[i];
  }
  double mx=sx/n,my=sy/n,vx=0,vy=0,cov=0;
  for(int i=0;i<n;i++){vx+=(x[i]-mx)*(x[i]-mx);vy+=(y[i]-my)*(y[i]-my);cov+=(x[i]-mx)*(y[i]-my);}
  vx/=n;vy/=n;cov/=n;
  char lines[9][48];
  snprintf(lines[0],48,"Effectif : %d",n);
  snprintf(lines[1],48,"Moyenne x : %.6g",mx);
  snprintf(lines[2],48,"Moyenne y : %.6g",my);
  snprintf(lines[3],48,"Variance x : %.6g",vx);
  snprintf(lines[4],48,"Variance y : %.6g",vy);
  snprintf(lines[5],48,"Ecart-type x : %.6g",sqrt(vx));
  snprintf(lines[6],48,"Ecart-type y : %.6g",sqrt(vy));
  snprintf(lines[7],48,"Covariance : %.6g",cov);
  int count=8;
  if(vx>0){double a=cov/vx,b=my-a*mx;
    snprintf(lines[8],48,"Droite : y=%.4gx%+.4g",a,b);count=9;
  }
  const char *ptr[9];for(int i=0;i<count;i++)ptr[i]=lines[i];
  page("NUAGE DE POINTS",ptr,count);
}
static void table(void){
  int r=integer("Nombre de lignes (1 a 4)"),c=integer("Nombre colonnes (1 a 4)");
  if(r<1||r>4||c<1||c>4)return;
  double t[4][4],tr[4]={0},tc[4]={0},total=0;
  for(int i=0;i<r;i++)for(int j=0;j<c;j++){
    char lab[32];snprintf(lab,sizeof(lab),"L%d C%d",i+1,j+1);
    t[i][j]=number(lab);if(isnan(t[i][j]))return;tr[i]+=t[i][j];tc[j]+=t[i][j];total+=t[i][j];
  }
  char lines[16][48];int k=0;
  snprintf(lines[k++],48,"Total general : %.6g",total);
  for(int i=0;i<r;i++)snprintf(lines[k++],48,"Total ligne %d : %.6g",i+1,tr[i]);
  for(int j=0;j<c;j++)snprintf(lines[k++],48,"Total colonne %d : %.6g",j+1,tc[j]);
  if(total!=0)for(int i=0;i<r;i++)for(int j=0;j<c;j++){
    snprintf(lines[k++],48,"F L%dC%d : %.4g%%",i+1,j+1,t[i][j]/total*100);
  }
  const char *ptr[16];for(int i=0;i<k;i++)ptr[i]=lines[i];
  page("TABLEAU DOUBLE ENTREE",ptr,k);
}
static void literal(void){
  const char *opts[]={"Image f(x)=ax+b","Antecedent f(x)=ax+b","Resoudre ax+b=c","Developper (ax+b)^2","Developper (ax-b)^2","Identite (ax-b)(ax+b)"};
  for(;;){
    int c=menu("CALCUL LITTERAL",opts,6);if(c<0)return;
    double a=number("Coefficient a"),b=number("Terme b");
    if(isnan(a)||isnan(b))continue;
    double x=0,v=0;char l1[48],l2[48];
    if(c==0){x=number("Valeur x");if(isnan(x))continue;v=a*x+b;snprintf(l1,48,"f(x) = %.7g",v);snprintf(l2,48,"f(x)=a*x+b");}
    else if(c==1){v=number("Valeur f(x)");if(isnan(v))continue;if(a==0){result1("ANTECEDENT","Pas d'antecedent unique");continue;}x=(v-b)/a;snprintf(l1,48,"Antecedent x = %.7g",x);snprintf(l2,48,"x=(y-b)/a");}
    else if(c==2){v=number("Valeur c");if(isnan(v))continue;if(a==0){result1("EQUATION",b==v?"Infinite solutions":"Aucune solution");continue;}x=(v-b)/a;snprintf(l1,48,"Solution x = %.7g",x);snprintf(l2,48,"x=(c-b)/a");}
    else if(c==3){snprintf(l1,48,"%.5gx^2 + %.5gx + %.5g",a*a,2*a*b,b*b);snprintf(l2,48,"(ax+b)^2");}
    else if(c==4){snprintf(l1,48,"%.5gx^2 - %.5gx + %.5g",a*a,2*a*b,b*b);snprintf(l2,48,"(ax-b)^2");}
    else {snprintf(l1,48,"%.5gx^2 - %.5g",a*a,b*b);snprintf(l2,48,"(ax-b)(ax+b)");}
    const char *ls[]={l2,l1};page("CALCUL LITTERAL",ls,2);
  }
}
int main(void){
  const char *items[]={"Nuage de points","Tableau double entree","Fractions","Pourcentages","Calcul litteral","Quitter"};
  for(;;){
    int c=menu("MATHS LYCEE",items,6);
    if(c==0)nuage();
    else if(c==1)table();
    else if(c==2)fractions();
    else if(c==3)percentages();
    else if(c==4)literal();
    else break;
  }
  frame("MATHS LYCEE");text(8,80,"A bientot !",GREEN,BG);eadk_timing_msleep(900);
  return 0;
}
