# ifndef LABEL_H
# define LABEL_H
# include <ostream>

using namespace std;

class Label {
	static unsigned _counter;
	unsigned _number;

public:
	Label();
	unsigned number() const;
};

ostream &operator <<(ostream &ostr, const Label &label);

# endif /* LABEL_H */
