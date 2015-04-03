// Couleurs de LEDs
int ledR = 5;
int ledV = 3;
int ledB = 6;
// Switch
int sw = 2;
// Bouton poussoir
int bp = 7;
// Potentiomètre
int pot = A0;


unsigned int valR=0;
unsigned int valV=0;
unsigned int valB=0;
unsigned int choix=0;
unsigned int valeur=0;
unsigned int s=0;
int mode = 0;
int nouveauMode = 0;

double teinte=0;
int    k=0;
int    x=0;
unsigned int valR_potMode=0;
unsigned int valV_potMode=0;
unsigned int valB_potMode=0;

void setup() {                
  pinMode(ledR, OUTPUT);  
  pinMode(ledV, OUTPUT);  
  pinMode(ledB, OUTPUT);
  pinMode(sw, INPUT);
  Serial.begin(115200);
  Serial.setTimeout(4);
  
  valR = 255;
  valV = 255;
  valB = 255;
  
  valR_potMode = 255;
  valV_potMode = 0;
  valB_potMode = 0;
  
  analogWrite( ledR , valR );
  analogWrite( ledV , valV );
  analogWrite( ledB , valB );
}

void loop(){
  
  nouveauMode = digitalRead( sw ); // == HIGH ? 1 : 0;
  
  if( mode != nouveauMode )
  {
    mode = nouveauMode;
    
    if( mode == 0 )
    {
      analogWrite( ledR , valR_potMode );
      analogWrite( ledV , valV_potMode );
      analogWrite( ledB , valB_potMode );
    }
    else
    {
      analogWrite( ledR , valR );
      analogWrite( ledV , valV );
      analogWrite( ledB , valB );
    }
  }
  
  if( mode == 1 )
  {
    s = (unsigned int) Serial.parseInt();
    
    if( s != 0 )
    {
      //Serial.print( "s" );
      //Serial.println( s );
      
      //valeur = ( s & 255 ) % 256;
      valeur = ( s << 8 ) >> 8;
      choix = ( s & 65280 ) >> 8;
      
      //Serial.print( "choix" );
      //Serial.println( choix );
      
      if( choix == 1 )
      {
        //Serial.print( "rouge" );
        //Serial.println( valeur );
        valR = valeur;
        analogWrite( ledR , valR );
      }
      else if( choix == 2 )
      {
        //Serial.print( "vert" );
        //Serial.println( valeur );
        valV = valeur;
        analogWrite( ledV , valV );
      }
      else if( choix == 3 )
      {
        //Serial.print( "bleu" );
        //Serial.println( valeur );
        valB = valeur;
        analogWrite( ledB , valB );
      }
    }
  }
  else
  {
    teinte = ( (double)analogRead( pot ) / 1023.0 * 6.0 );
    
    k = (int)( floor( teinte ) );
    k = ( k%6  + 6 ) % 6;

    x = ( (int)floor( (teinte - (double)k) *255.0) % 255  + 255 ) % 255;
    
  
    valR_potMode = 0;
    valV_potMode = 0;
    valB_potMode = 0;
  
    // ROUGE VERS JAUNE
    if( k == 0 )
    {
      valR_potMode = 255;
      valV_potMode = x;
    }
    // JAUNE VERS VERT
    else if( k == 1 )
    {
      valR_potMode = 255 - x;
      valV_potMode = 255;
    }
    // VERT VERS CYAN
    else if( k == 2 )
    {
      valV_potMode = 255;
      valB_potMode = x;
    }
    // CYAN VERS BLEU
    else if( k == 3 )
    {
      valV_potMode = 255 - x;
      valB_potMode = 255;
    }
    // BLEU VERS MAGENTA
    else if( k == 4 )
    {
      valR_potMode = x;
      valB_potMode = 255;
    }
    // MAGENTA VERS ROUGE
    else
    {
      valR_potMode = 255;
      valB_potMode = 255 - x;
    }
    
    analogWrite( ledR , valR_potMode );
    analogWrite( ledV , valV_potMode );
    analogWrite( ledB , valB_potMode );
    /*
    Serial.print( "pot : " );
    Serial.println( ( (double)analogRead( pot ) / 1023.0 * 100.0 ) );
    */
    //Serial.println( "pot : " );
  }
}

