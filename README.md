kRPCI
=====

kRPCI is a C++ client to kRPC for communicating with Kerbal Space Program (KSP). 

kRPCI Generator
---------------

* The kRPC interface - krpci_generated.cpp and krpci.hpp files are completely generated from Cheetah templates in templates/*.tmpl
* Use krpci_generator.py to regenerate the client.
* To test, you can use krpci_test.cpp and the Makefile

Cheetah Templating Engine
-------------------------

* Regeneration requires the Cheetah Templating Engine.
* Download from: https://pypi.python.org/pypi/Cheetah/2.4.4
* Extract this Cheetah-2.4.4.tar.gz
* Install:

```bash
cd Cheetah-2.4.4
sudo python setup.py install
```

Other Assumptions
-----------------

* We expect KSP & kRPC server to be running
* We expect an active vessel in flight (in focus, in the game)
* The script fetches all the services & procedures using the KSP interface and generates the API we need.
* Then, krpci_test.cpp uses the generated KRPCI API (krpci_generated.cpp & krpci.hpp) to communicate with KSP

