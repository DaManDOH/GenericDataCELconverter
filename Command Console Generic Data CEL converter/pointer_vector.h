#ifndef __blah__
#define __blah__

#include <vector>

template <typename T>
class pointer_vector : protected std::vector<T*> {
	~pointer_vector();
};

template <typename T>
pointer_vector<T>::~pointer_vector() {
}

#endif /* __blah__ */