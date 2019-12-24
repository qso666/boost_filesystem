#include "Header.h"
File::File(std::string br, std::string filename)
	: _broker(br),
	_account(filename.substr(8, 8)),
	_count(1),
	_last_date(filename.substr(17, 8)) {}

std::ostream& operator<<(std::ostream& out, const File& file) {
	out << "broker:" << file._broker << " account:" << file._account
		<< " files:" << file._count << " lastdate:" << file._last_date;
	return out;
}

void scan_dir(const path& p, std::vector<File>& vec) {
	if (!exists(p)) {
		std::cout << "does not exist";
		exit(2);
	}
	for (auto& x : directory_iterator{ p }) {
		if (boost::filesystem::is_regular_file(x.path())) {
			std::string filename = x.path().filename().string();
			if (filename.length() != 29) continue;
			if (filename.substr(0, 8) != "balance_") continue;
			for (size_t i = 8; i < 16; i++) {
				if (filename[i] < '0' || filename[i] > '9') continue;
			}
			if (filename[16] != '_') continue;
			for (size_t i = 17; i < 25; i++) {
				if (filename[i] < '0' || filename[i] > '9') continue;
			}
			int year = 1000 * (filename[17] - 48) + 10 * (filename[18] - 48) +
				10 * (filename[19] - 48) + filename[20] - 48;
			int month = 10 * (filename[21] - 48) + filename[22] - 48;
			int day = 10 * (filename[23] - 48) + filename[24] - 48;
			if (year < 1970 || year > 2019) continue;
			if (month < 0 || month > 12) continue;
			if (day < 0) continue;
			if (month == 4 || month == 6 || month == 9 || month == 11) {
				if (day > 30) continue;
			}
			else if (month == 2) {
				if (year % 4 == 0) {
					if (day > 29) continue;
				}
				else {
					if (day > 28) continue;
				}
			}
			else {
				if (day > 31) continue;
			}
			if (filename[25] != '.') continue;
			std::cout << p.filename().string() << " " << filename << std::endl;
			File new_file(p.filename().string(), filename);
			bool inserted = false;
			for (auto& pos : vec) {
				if (pos._broker == new_file._broker) {
					if (pos._account == new_file._account) {
						pos._count++;
						pos._last_date = pos._last_date > new_file._last_date
							? pos._last_date
							: new_file._last_date;
						inserted = true;
					}
				}
			}
			if (!inserted) {
				vec.push_back(new_file);
			}
		}
		else if (boost::filesystem::is_directory(x.path())) {
			scan_dir(x.path(), vec);
		}
	}
}

int main() {
	std::vector<File> files;
	const boost::filesystem::path& p = "D:\\ftp";
	scan_dir(p, files);
	for (auto pos : files) std::cout << std::endl << pos;
	return 0;
}