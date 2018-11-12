#include <cmath>
#include <memory>
#include <iostream>
#include <fstream>
using namespace std;

struct Vector {
	float x, y, z;

	float length() const {
		return sqrtf(x * x + y * y + z * z);
	}

	Vector operator-() const {
		return Vector{-x, -y, -z};
	}
};

Vector operator*(const Vector & v, float scale) {
	return Vector{v.x * scale, v.y * scale, v.z * scale};
}

Vector operator+(const Vector & v, const Vector &r) {
	return Vector{v.x + r.x, v.y + r.y, v.z + r.z};
}

bool operator<=(const Vector & v, const Vector &r) {
	return v.x <= v.x && v.y <= v.y && v.z <= v.z;
}

std::ostream &operator<<(std::ostream &out, const Vector &v) {
	return out << v.x << ' ' << v.y << ' ' << v.z;
}

std::istream &operator >> (std::istream &in, Vector &v) {
	return in >> v.x >> v.y >> v.z;
}

class Figure {
protected:
	Vector center;
public:
	Figure(const Vector & center): center(center) {}

	virtual Figure * clone() const = 0;
	virtual bool isInside(const Vector &point) const = 0;
	virtual void print(std::ostream &out) const = 0;
	virtual void printIfInside(const Vector &point, std::ostream &out) const {
		if (isInside(point)) {
			print(out);
			out << endl;
		}
	}

	virtual ~Figure() {}
};

class Sphere: public Figure {
	float radius;
public:
	Sphere(const Vector center, float radius): Figure(center), radius(radius) {}

	Figure * clone() const override {
		return new Sphere(center, radius);
	}

	void print(std::ostream &out) const override {
		out << "Sphere at (" << center << ") radius " << radius;
	}

	bool isInside(const Vector &point) const override {
		const Vector translated = point + -center;
		return translated.length() <= radius;
	}
};

class Cuboid: public Figure {
	Vector lengths;
public:
	Cuboid(const Vector &center, const Vector lengths): Figure(center), lengths(lengths) {}

	Figure * clone() const override {
		return new Cuboid(center, lengths);
	}

	void print(std::ostream &out) const override {
		out << "Cuboid at (" << center << ") with sides " << lengths;
	}

	bool isInside(const Vector &p) const override {
		const Vector min = center + -(lengths * 0.5);
		const Vector max = center + (lengths * 0.5);
		return min <= p && p <= max;
	}
};

class Group: public Figure {
	Figure ** elements;
	int count;
	float scale;
public:
	Group(const Vector &translate, float scale): Figure(translate), scale(scale), elements(nullptr), count(0) {}

	Figure * clone() const override {
		Group * grp = new Group(center, scale);
		grp->elements = new Figure*[count];
		grp->count = count;
		for (int c = 0; c < count; c++) {
			grp->elements[c] = elements[c]->clone();
		}
		return grp;
	}

	void print(std::ostream &out) const override {
		// do nothing - we are group
	}

	bool isInside(const Vector &point) const override {
		// we are forced to implement this
		return true;
	}

	void printIfInside(const Vector &point, std::ostream &out) const override {
		// transform point with opposite of Group transfrom so we can check correctly
		const Vector translated = point + (-center);
		const Vector final = translated * (1 / scale);
		for (int c = 0; c < count; c++) {
			elements[c]->printIfInside(final, out);
		}
	}

	void addFigure(Figure & fig) {
		// this could grow smarter
		Figure ** figs = new Figure*[count + 1];
		memcpy(figs, elements, count * sizeof(Figure*));
		delete[] elements;
		elements = figs;
		elements[count++] = fig.clone();
	}

	~Group() {
		for (int c = 0; c < count; c++) {
			delete[] elements[c];
		}
		delete[] elements;
	}
};

void readRecursive(std::istream & in, Group & grp) {
	char typeBuff[32];
	while (true) {
		if (!(in >> typeBuff)) {
			return;
		}
		if (strcmp(typeBuff, "sphere") == 0) {
			Vector center;
			float radius;
			in >> center >> radius;
			Sphere sphere(center, radius);
			grp.addFigure(sphere);
		} else if (strcmp(typeBuff, "cuboid") == 0) {
			Vector center, sizes;
			in >> center >> sizes;
			Cuboid cuboid(center, sizes);
			grp.addFigure(cuboid);
		} else {
			in >> typeBuff;
			if (strcmp(typeBuff, "out") == 0) {
				return;
			} else {
				Vector translate;
				float scale;
				in >> translate >> scale;
				Group innerGrp(translate, scale);
				readRecursive(in, innerGrp);
				grp.addFigure(innerGrp);
			}
		}
	}
}

// wrap all figures in a group so we can easily query printIfInside
Figure * readInput(std::istream & in) {
	Group * all = new Group(Vector{0, 0, 0}, 1);
	readRecursive(in, *all);
	return all;
}

int main(int argc, char * argv[]) {
	if (argc != 2) {
		std::cerr << "Pass input file path as first argument to the program";
		return -1;
	}
	std::cout << "Will open file" << argv[1];
	std::fstream input(argv[1]);
	Figure * figs = readInput(input);
	input.close();
	while (1) {
		std::cout << "Enter point coords:";
		Vector v;
		std::cin >> v;
		figs->printIfInside(v, std::cout);
	}
	return 0;
}