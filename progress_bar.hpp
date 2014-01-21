#ifndef _PROGRESS_BAR_
#define _PROGRESS_BAR_

class ProgressBar{

    public:        
        ProgressBar();
        ProgressBar(int n_, const char *description_="");
		
		void SetFrequencyUpdate(int frequency_update_);
		void SetStyle(const char* unit_bar_, const char* unit_space_);		
		int SetLengthFromConsoleWidth();

        void ClearBarField();
		
        void Progressed(unsigned int idx_);
		
    private:
	
		int n;
        const char *description;

		int desc_width;
		
		const char *unit_bar;
        const char *unit_space;
		
		int frequency_update;

};

#endif
