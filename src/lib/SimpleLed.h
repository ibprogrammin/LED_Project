#ifndef SIMPLELED_H
#define SIMPLELED_H

class SimpleLed {
    public:
    SimpleLed();
    void load();
    void run();
    void setFrequency(int frequency);


    private:
    int frequency;

    const int DEFAULT_FREQUENCY = 5;
    const int LEDPIN = 18;

    void chase();
    void dimUpDown();
    void flash();
    void strobe();
    void temperatureSesitive();
    
    //static double dim_value = 0.0; // MAX 255
    //static int increment = 0;
    //static int counter = 0;
};


#endif