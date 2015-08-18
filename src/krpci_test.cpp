#include "krpci.hpp"

using namespace std;

// FORWARD DECLARATIONS REQUIRED FOR STREAM TESTING
class myclass
{
public:
  myclass(int tmp=10) : myInstVar(tmp) {}
  // SHOWING HOW A FUNCTION OF A CLASS CAN BE CALLED EVERY TIME A RESPONSE
  // IS RECEIVED ON A SPECIFIC STREAM - IN THIS CASE A VELOCITY DATA STREAM
  void classStreamFunc(krpc::Response& response)
  {
    if ( response.has_error() )
      {
	std::cout << "Response error: " << response.error() << endl;
	return;
      }
    // velocity
    double x,y,z;
    KRPCI::Vessel_Velocity_parseResponse(response, x, y, z);
    printf("CLASS METHOD WITH INST VAR %d : (x,y,z) = (%f,%f,%f)\n",myInstVar,x,y,z);
  }
  int myInstVar;
};

// SHOWING HOW A REGULAR FUNCTION CAN BE CALLED EVERY TIME A RESPONSE
// IS RECEIVED ON A SPECIFIC STREAM - IN THIS CASE A POSITION DATA STREAM
void myStreamFunc(krpc::Response& response)
{
  if ( response.has_error() )
    {
      std::cout << "Response error: " << response.error() << endl;
      return;
    }
  // position
  double x,y,z;
  KRPCI::Vessel_Position_parseResponse(response, x, y, z);
  printf("(x,y,z) = (%f,%f,%f)\n",x,y,z);
}

// THE MAIN TEST CODE
int main(int argc, char** argv)
{
  KRPCI client("wrapperTest");
  if ( client.Connect() )
    {
      // TEST SENSING: GETTING VESSEL, ORBIT, BODY PROPERTIES/CHILDREN
      std::vector<uint64_t> vesselIDs;
      uint64_t vesselID;
      std::string vesselName;
      uint64_t orbitID;
      double apoapsis;
      double time;
      uint64_t bodyID;
      uint64_t orbitalRefFrame;
      double position[3];
      double velocity[3];
      uint64_t controlID;

      // GET DATA FROM KRPC
      client.get_Vessels(vesselIDs);
      client.get_ActiveVessel(vesselID);
      client.Vessel_get_Name(vesselID,vesselName);
      client.Vessel_get_Orbit(vesselID, orbitID);
      client.Orbit_get_ApoapsisAltitude(orbitID,apoapsis);
      client.Orbit_get_TimeToApoapsis(orbitID,time);
      client.Orbit_get_Body(orbitID, bodyID);
      client.CelestialBody_get_ReferenceFrame(bodyID, orbitalRefFrame);
      client.Vessel_Position(vesselID, orbitalRefFrame, position[0], position[1], position[2]);
      client.Vessel_Velocity(vesselID, orbitalRefFrame, velocity[0], velocity[1], velocity[2]);
      client.Vessel_get_Control(vesselID, controlID);

      // PRINT RETRIEVED DATA
      std::cout << "There are " << vesselIDs.size() << " vessel(s):" << endl;
      for (int i=0;i<vesselIDs.size();i++)
	{
	  std::cout << "\tID #" << i+1 << " = " << vesselIDs[i] << endl;
	}
      std::cout << "Active vessel ID: " << vesselID << std::endl;
      std::cout << "Active vessel Name: " << vesselName << std::endl;
      std::cout << "Active vessel orbit: " << orbitID << endl;
      std::cout << "Active vessel orbit has apoapsis altitude: " << apoapsis << endl;
      std::cout << "Active vessel reaches orbit apoapsis in: " << time << endl;
      std::cout << "Active vessel orbiting body: " << bodyID << std::endl;
      std::cout << "Orbiting body's Reference Frame ID: " << orbitalRefFrame << std::endl;
      std::cout << "Active vessel Position: "<< position[0]<<","<<position[1]<<","<<position[2]<<endl;
      std::cout << "Active vessel Velocity: "<< velocity[0]<<","<<velocity[1]<<","<<velocity[2]<<endl;
      std::cout << "Active vessel has control ID: " << controlID << endl;

      // TEST CONTROLS : NEED A CONTROL OBJECT (ID) ASSOCIATED WITH A VESSEL
      client.Control_set_SAS(controlID,true);
      client.Control_set_RCS(controlID,true);
      client.Control_set_Throttle(controlID,1.0);
      client.Control_set_Pitch(controlID,45.0);
      client.Control_set_Roll(controlID,20.0);
      client.Control_set_Yaw(controlID,30.0);

      // TEST STREAMS: THESE ARE NOT CRITICAL, BUT CAN BE USEFUL
      // AND ARE PROVIDED BY KRPC TO ALLOW PERIODIC REQUEST INVOCATION
      // stream name can be anything, but must be unique for a given client (i.e. KRPCI)
      std::string streamName = "streamTest_Vessel_Position";
      krpc::Request request;
      KRPCI::Vessel_Position_createRequest(vesselID, orbitalRefFrame, request);

      client.CreateStream(streamName,request, myStreamFunc);

      
      myclass class1 = myclass(50);
      std::string streamName2 = "streamtest_Vessel_Velocity";
      krpc::Request request2;
      KRPCI::Vessel_Velocity_createRequest(vesselID, orbitalRefFrame, request2);

      client.CreateStream(streamName2, request2, boost::bind(&myclass::classStreamFunc, class1, _1));

      sleep(10);

      client.RemoveStream(streamName);

      sleep(10);
      // DONE TESTING STREAMS
    }
  client.Close();
}

