#ifndef STORAGE_H
#define STORAGE_H

template <typename T>
class Storage 
{
private:
    T* data[100];
    int count;

public:
    Storage() : count(0) 
    {
        for (int i = 0; i < 100; i++) 
        {
            data[i] = nullptr;
        }
    }

    void add(T* item) 
    {
        if (count < 100 && item != nullptr) 
        {
            data[count++] = item;
        }
    }

    T* findByID(int id) 
    {
        for (int i = 0; i < count; i++) 
        {
            if (data[i]->getID() == id) 
            {
                return data[i];
            }
        }
        return nullptr;
    }

    bool removeByID(int id) 
    {
        for (int i = 0; i < count; i++) 
        {
            if (data[i]->getID() == id) 
            {
                delete data[i]; 

                for (int j = i; j < count - 1; j++) 
                {
                    data[j] = data[j + 1];
                }
                data[count-1] = nullptr;
                count--;
                return true;
            }
        }
        return false;
    }

    T* getDataAtIndex(int index) 
    {
        if (index >= 0 && index < count) 
        {
            return data[index];
        }
        else 
        {
            return nullptr;
        }
    }

    int size() const 
    {
        return count;
    }

    ~Storage() 
    {
        for (int i = 0; i < count; i++) 
        {
            delete data[i];
        }
    }
};
#endif