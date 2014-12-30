#ifndef _PROGRESS_BAR_
#define _PROGRESS_BAR_

class ProgressBar{

    public:        
        ProgressBar();
        ProgressBar(unsigned long n_, const char *description_="");
		
		void SetFrequencyUpdate(unsigned long frequency_update_);
		void SetStyle(const char* unit_bar_, const char* unit_space_);		

        void Progressed(unsigned long idx_);
		
    private:
	
		unsigned long n;
		unsigned int desc_width;
		unsigned long frequency_update;
		
		const char *description;
		const char *unit_bar;
        const char *unit_space;
		
		void ClearBarField();
		int GetConsoleWidth();
		int GetBarLength();

};

#endif
