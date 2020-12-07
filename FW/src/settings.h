#ifndef __SETTINGS_H
#define __SETTINGS_H

class SettingsManager
{
	public:
		bool begin();

		void write_flash(double*);
		void read_flash(double*);

	private:
};

#endif //__SETTINGS_H

