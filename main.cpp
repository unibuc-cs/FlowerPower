#include <iostream>
#include <fstream>
#include <map>
#include <vector>
using namespace std;

class Sensor
{
    string name;
    double doubleValue;
    string stringValue;
    double minValue;
    double maxValue;
public:
    Sensor()
    {

    }
    Sensor(string _name, double _value, double _minValue, double _maxValue)
    {
        name = _name;
        doubleValue = _value;
        minValue = _minValue;
        maxValue = _maxValue;
    }
    Sensor(string _name, string _value, double _minValue, double _maxValue)
    {
        name = _name;
        stringValue = _value;
        minValue = _minValue;
        maxValue = _maxValue;
    }
    ~Sensor()
    {

    }
    void SetName(string newName)
    {
        name = newName;
    }
    string GetName()
    {
        return name;
    }
    void SetValue(double newValue)
    {
        doubleValue = newValue;
    }
    double GetDoubleValue()
    {
        return doubleValue;
    }
    void SetValue(string newValue)
    {
        stringValue = newValue;
    }
    string GetStringValue()
    {
        return stringValue;
    }
    void SetMinValue(double newValue)
    {
        minValue = newValue;
    }
    double GetMinValue()
    {
        return minValue;
    }
    void SetMaxValue(double newValue)
    {
        maxValue = newValue;
    }
    double GetMaxValue()
    {
        return maxValue;
    }

};

class Plant
{
    string name;
    string color;
    double height;
    string plantType;
    string suitableSoilType;
public:
    Plant(string _name, string _color, double _height, string _plantType, string _suitableSoilType)
    {
        name = _name;
        color = _color;
        height = _height;
        plantType = _plantType;
        suitableSoilType = _suitableSoilType;
    }
    Plant()
    {

    }
    ~Plant()
    {

    }
    string GetName()
    {
        return name;
    }
    string GetColor()
    {
        return color;
    }
    double GetHeight()
    {
        return height;
    }
    string GetType()
    {
        return plantType;
    }
    string GetSoil()
    {
        return suitableSoilType;
    }
};
bool operator==(Plant& p1, Plant& p2)
{
    return p1.GetName() == p2.GetName();
}
class SmartPot
{
    Plant plant;
    map<string,Sensor> settings;
public:
    SmartPot(Plant _plant, vector<Sensor> sensors)
    {
        plant = _plant;
        for(vector<Sensor>::iterator it = sensors.begin(); it != sensors.end(); ++it)
        {
            settings[it->GetName()] = (*it);
        }
    }
    int Get(const string& name, Sensor& returnedValue)
    {
        // If the setting does not exist.
        if(settings.find(name) == settings.end())
        {
            returnedValue = Sensor();
            return 1;
        }
        else
        {
            returnedValue = settings[name];
        }
        return 0;
    }

    int Set(const string& name, const Sensor& value)
    {
        // If the setting does not exist.
        if(settings.find(name) == settings.end())
        {
            return 1;
        }
        else
        {
            settings[name] = value;
        }

        return 0;
    }

    string Shovel()
    {
        return("0%Soil has been shovelled!");
    }
    string Moisturise()
    {
        if(settings.find("soilHumidity") == settings.end())
            return "-1%No soilHumidity sensor found!";
        Sensor soilHumidity = settings["soilHumidity"];
        if(soilHumidity.GetDoubleValue() < soilHumidity.GetMinValue())
        {
            soilHumidity.SetValue(soilHumidity.GetMaxValue());
            return ("0%Soil has been moistened, current soil humidity: " + to_string(soilHumidity.GetDoubleValue()));
        }
        return "0%";
    }
    string NutrientsInjector()
    {
        string returnMessage = "0%";
        string nutrientsInjected = "";
        if(settings.find("phosphorus") == settings.end())
            return "-1%No phosphorus found!";
        if(settings.find("nitrogen") == settings.end())
            return "-1%No nitrogen found!";
        if(settings.find("potassium") == settings.end())
            return "-1%No potassium found!";
        Sensor ph = settings["phosphorus"];
        Sensor n = settings["nitrogen"];
        Sensor p = settings["potassium"];
        if(ph.GetDoubleValue() < ph.GetMinValue())
        {
            ph.SetValue(ph.GetMaxValue());
            nutrientsInjected += "phosphorus, ";
        }
        if(n.GetDoubleValue() < n.GetMinValue())
        {
            n.SetValue(n.GetMaxValue());
            nutrientsInjected += "nitrogen, ";
        }
        if(p.GetDoubleValue() < p.GetMinValue())
        {
            p.SetValue(p.GetMaxValue());
            nutrientsInjected += "potassium, ";
        }
        if(nutrientsInjected.compare("") == 0)
        {
            return returnMessage;
        }
        nutrientsInjected = nutrientsInjected.substr(0, nutrientsInjected.size() - 2);
        returnMessage += "Nutrients injected: " + nutrientsInjected;
        return returnMessage;
    }
    string SolarLamp()
    {
        string returnMessage = "0%";
        if(settings.find("luminosity") == settings.end())
            return "-1%No luminosity sensor found!";
        Sensor luminosity = settings["luminosity"];
        if(luminosity.GetDoubleValue() < luminosity.GetMinValue())
        {
            luminosity.SetValue((luminosity.GetMinValue() + luminosity.GetMaxValue())/2);
            returnMessage += "Luminosity has been increased to: " + to_string(luminosity.GetDoubleValue());
        }
        return returnMessage;
    }

    string DisplayPlantData()
    {
        Plant p;
        if(plant == p)
            return "-1%No plant found!";
        string returnMessage = "0%";
        returnMessage += "Plant species: " + plant.GetName() + "\nPlant color: " + plant.GetColor() + "\nPlant height: " + to_string(plant.GetHeight()) + "\nPlant type: " + plant.GetType();

        return returnMessage;
    }
    string DisplayEnviornmentData()
    {
        string returnMessage = "0%";
        /*if(settings.find("airHumidity") == settings.end())
            return "-1%No airHumidity sensor found!";
        returnMessage += "Air humidity: " + to_string(settings["airHumidity"].GetDoubleValue());
        if(settings.find("airTemperature") == settings.end())
            return "-1%No airTemperature sensor found!";
        returnMessage += "Air temperature: " + to_string(settings["airTemperature"].GetDoubleValue());
        if(settings.find("luminosity") == settings.end())
            return "-1%No luminosity sensor found!";
        returnMessage += "Luminosity: " + to_string(settings["luminosity"].GetDoubleValue());
        if(settings.find("soilHumidity") == settings.end())
            return "-1%No soilHumidity sensor found!";
        returnMessage += "Soil humidity: " + to_string(settings["soilHumidity"].GetDoubleValue());
        if(settings.find("soilType") == settings.end())
            return "-1%No soilType sensor found!";
        returnMessage += "Soil type: " + settings["airHumidity"].GetStringValue();
        if(settings.find("soilPh") == settings.end())
            return "-1%No soilPh sensor found!";
        returnMessage += "soilPh: " + to_string(settings["soilPh"].GetDoubleValue());*/
        for (auto it = settings.begin(); it != settings.end(); ++it)
        {
            Sensor s = it->second;
            if(s.GetStringValue().compare("") != 0)
                returnMessage += "\n" + s.GetName() + ": " + s.GetStringValue();
            else
                returnMessage += "\n" + s.GetName() + ": " + to_string(s.GetDoubleValue());
        }
        return returnMessage;
    }
    string SoilCompatibility()
    {
        Plant p;
        if(plant == p)
            return "-1%No plant found!";
        if(settings.find("soilType") == settings.end())
            return "-1%No soilType sensor found!";
        if(plant.GetSoil().compare(settings["soilType"].GetStringValue()) != 0)
            return "1%Soil Type not suitable for plant!";
        else
            return "0%";
    }
    string CriticalSoilLevels()
    {
        bool alert = false;
        string alertMessages = "";
        if(settings.find("soilPh") == settings.end())
            return "-1%No soilPh sensor found!";
        if(settings.find("soilHumidity") == settings.end())
            return "-1%No soilHumidity sensor found!";
        Sensor soilPh = settings["soilPh"];
        Sensor soilHumidity = settings["soilHumidity"];
        if(soilPh.GetDoubleValue() < soilPh.GetMinValue())
        {
            alert = true;
            alertMessages += "Soil ph under critical levels!";
        }
        else if(soilPh.GetDoubleValue() > soilPh.GetMaxValue())
        {
            alert = true;
            alertMessages += "Soil ph above critical levels!";
        }
        if(soilHumidity.GetDoubleValue() < soilHumidity.GetMinValue())
        {
            alert = true;
            alertMessages += "Soil humidity under critical levels!";
        }
        else if(soilHumidity.GetDoubleValue() > soilHumidity.GetMaxValue())
        {
            alert = true;
            alertMessages += "Soil humidity above critical levels!";
        }
        if(alert)
            return "1%" + alertMessages;
        else
            return "0%";
    }
    string InadequateEnvironment()
    {
        bool alert = false;
        string alertMessages = "";
        if(settings.find("temperature") == settings.end())
            return "-1%No temperature sensor found!";
        if(settings.find("humidity") == settings.end())
            return "-1%No humidity sensor found!";
        Sensor temperature = settings["temperature"];
        Sensor humidity = settings["humidity"];
        if(temperature.GetDoubleValue() < temperature.GetMinValue())
        {
            alert = true;
            alertMessages += "Temperature under critical levels!";
        }
        else if(temperature.GetDoubleValue() > temperature.GetMaxValue())
        {
            alert = true;
            alertMessages += "Temperature above critical levels!";
        }
        if(humidity.GetDoubleValue() < humidity.GetMinValue())
        {
            alert = true;
            alertMessages += "Humidity under critical levels!";
        }
        else if(humidity.GetDoubleValue() > humidity.GetMaxValue())
        {
            alert = true;
            alertMessages += "Humidity above critical levels!";
        }
        if(alert)
            return "1%" + alertMessages;
        else
            return "0%";
    }

};




int main()
{
    vector<Sensor> s;
    Sensor s1("soilHumidity", 2,3,6);
    Sensor s2("luminosity", 2,4,5);
    Sensor s3("temperature", 3,3,3);
    Sensor s4("soilType", "Red",3,3);
    Sensor s5("humidity", 3,3,3);
    Sensor s6("soilPh", 3,3,3);
    s.push_back(s1);
    s.push_back(s2);
    s.push_back(s3);
    s.push_back(s4);
    s.push_back(s5);
    s.push_back(s6);
    Plant p("Cactus", "Green", 1.3, "Desert", "Red");
    SmartPot *sp = new SmartPot(p, s);
    cout<<sp->SoilCompatibility()<<endl;
    cout<<sp->InadequateEnvironment()<<endl;
    cout<<sp->CriticalSoilLevels()<<endl;
    return 0;
}
