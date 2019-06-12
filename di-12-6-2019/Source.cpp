
#include <vector>
#include <map>

class LightInterface
{
	
};

class GeometryInterface
{
	
};

const int LIGHT_INTERFACE_ID = 5;
const int GEOM_INTERFACE_ID = 6;

class Plugin
{
	virtual Plugin *getInteface(int id) {
		return nullptr;
	}

	virtual Plugin* addInterface(int id, Plugin *p) {
		
	}

	virtual Plugin* queryInterface(int id) {
		
	}

	std::map<int, Plugin*> runtimeInteface;
};


class MyObject : Plugin, GeometryInterface
{
	Plugin* getInterface(int id) {
		if (id == GEOM_INTERFACE_ID) {
			return this;
		}
	}
};


///////

class MyNewInteface
{
	
};
int MY_INTERFACE = 7;



MyObject a;
a.addInteface(MY_INTERFACE, new MyNewInteface);





