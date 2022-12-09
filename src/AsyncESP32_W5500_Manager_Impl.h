/****************************************************************************************************************************
  AsyncESP32_W5500_Manager_Impl.h

  For Ethernet shields using ESP32_W5500 (ESP32 + W5500)

  WebServer_ESP32_W5500 is a library for the ESP32 with Ethernet W5500 to run WebServer

  Based on and modified from ESP32-IDF https://github.com/espressif/esp-idf
  Built by Khoi Hoang https://github.com/khoih-prog/WebServer_ESP32_W5500

  Modified from
  1. Tzapu               (https://github.com/tzapu/WiFiManager)
  2. Ken Taylor          (https://github.com/kentaylor)
  3. Alan Steremberg     (https://github.com/alanswx/ESPAsyncWiFiManager)
  4. Khoi Hoang          (https://github.com/khoih-prog/ESPAsync_WiFiManager)

  Built by Khoi Hoang https://github.com/khoih-prog/AsyncESP32_W5500_Manager
  Licensed under MIT license

  Version: 1.0.0

  Version Modified By  Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang     07/12/2022 Initial coding for ESP32_W5500
 *****************************************************************************************************************************/

#pragma once

#ifndef AsyncESP32_W5500_Manager_Impl_h
#define AsyncESP32_W5500_Manager_Impl_h

//////////////////////////////////////////

ESPAsync_EMParameter::ESPAsync_EMParameter(const char *custom)
{
  _WMParam_data._id = NULL;
  _WMParam_data._placeholder = NULL;
  _WMParam_data._length = 0;
  _WMParam_data._value = NULL;
  _WMParam_data._labelPlacement = WFM_LABEL_BEFORE;

  _customHTML = custom;
}

//////////////////////////////////////////

ESPAsync_EMParameter::ESPAsync_EMParameter(const char *id, const char *placeholder, const char *defaultValue,
                                           const int& length, const char *custom, const int& labelPlacement)
{
  init(id, placeholder, defaultValue, length, custom, labelPlacement);
}

//////////////////////////////////////////

// KH, using struct
ESPAsync_EMParameter::ESPAsync_EMParameter(const WMParam_Data& WMParam_data)
{
  init(WMParam_data._id, WMParam_data._placeholder, WMParam_data._value,
       WMParam_data._length, "", WMParam_data._labelPlacement);
}

//////////////////////////////////////////

void ESPAsync_EMParameter::init(const char *id, const char *placeholder, const char *defaultValue,
                                const int& length, const char *custom, const int& labelPlacement)
{
  _WMParam_data._id = id;
  _WMParam_data._placeholder = placeholder;
  _WMParam_data._length = length;
  _WMParam_data._labelPlacement = labelPlacement;

  _WMParam_data._value = new char[_WMParam_data._length + 1];

  if (_WMParam_data._value != NULL)
  {
    memset(_WMParam_data._value, 0, _WMParam_data._length + 1);

    if (defaultValue != NULL)
    {
      strncpy(_WMParam_data._value, defaultValue, _WMParam_data._length);
    }
  }

  _customHTML = custom;
}

//////////////////////////////////////////

ESPAsync_EMParameter::~ESPAsync_EMParameter()
{
  if (_WMParam_data._value != NULL)
  {
    delete[] _WMParam_data._value;
  }
}

//////////////////////////////////////////

// Using Struct to get/set whole data at once
void ESPAsync_EMParameter::setWMParam_Data(const WMParam_Data& WMParam_data)
{
  LOGINFO(F("setWMParam_Data"));

  memcpy(&_WMParam_data, &WMParam_data, sizeof(_WMParam_data));
}

//////////////////////////////////////////

void ESPAsync_EMParameter::getWMParam_Data(WMParam_Data& WMParam_data)
{
  LOGINFO(F("getWMParam_Data"));

  memcpy(&WMParam_data, &_WMParam_data, sizeof(WMParam_data));
}

//////////////////////////////////////////

const char* ESPAsync_EMParameter::getValue()
{
  return _WMParam_data._value;
}

//////////////////////////////////////////

const char* ESPAsync_EMParameter::getID()
{
  return _WMParam_data._id;
}

//////////////////////////////////////////

const char* ESPAsync_EMParameter::getPlaceholder()
{
  return _WMParam_data._placeholder;
}

//////////////////////////////////////////

int ESPAsync_EMParameter::getValueLength()
{
  return _WMParam_data._length;
}

//////////////////////////////////////////

int ESPAsync_EMParameter::getLabelPlacement()
{
  return _WMParam_data._labelPlacement;
}

//////////////////////////////////////////

const char* ESPAsync_EMParameter::getCustomHTML()
{
  return _customHTML;
}

//////////////////////////////////////////

/**
   [getParameters description]
   @access public
*/
ESPAsync_EMParameter** AsyncESP32_W5500_Manager::getParameters()
{
  return _params;
}

//////////////////////////////////////////
//////////////////////////////////////////

/**
   [getParametersCount description]
   @access public
*/
int AsyncESP32_W5500_Manager::getParametersCount()
{
  return _paramsCount;
}

//////////////////////////////////////////

char* AsyncESP32_W5500_Manager::getRFC952_hostname(const char* iHostname)
{
  memset(RFC952_hostname, 0, sizeof(RFC952_hostname));

  size_t len = (RFC952_HOSTNAME_MAXLEN < strlen(iHostname)) ? RFC952_HOSTNAME_MAXLEN : strlen(iHostname);

  size_t j = 0;

  for (size_t i = 0; i < len - 1; i++)
  {
    if (isalnum(iHostname[i]) || iHostname[i] == '-')
    {
      RFC952_hostname[j] = iHostname[i];
      j++;
    }
  }

  // no '-' as last char
  if (isalnum(iHostname[len - 1]) || (iHostname[len - 1] != '-'))
    RFC952_hostname[j] = iHostname[len - 1];

  return RFC952_hostname;
}

//////////////////////////////////////////

AsyncESP32_W5500_Manager::AsyncESP32_W5500_Manager(AsyncWebServer * webserver, AsyncDNSServer *dnsserver,
                                                   const char *iHostname)
{

  server    = webserver;
  dnsServer = dnsserver;

#if USE_DYNAMIC_PARAMS
  _max_params = ETH_MANAGER_MAX_PARAMS;
  _params = (ESPAsync_EMParameter**) malloc(_max_params * sizeof(ESPAsync_EMParameter*));
#endif

  if (iHostname[0] == 0)
  {
    String _hostname = "ESP32-" + String(ESP_getChipId(), HEX);

    _hostname.toUpperCase();

    getRFC952_hostname(_hostname.c_str());

  }
  else
  {
    // Prepare and store the hostname only not NULL
    getRFC952_hostname(iHostname);
  }

  LOGWARN1(F("RFC925 Hostname ="), RFC952_hostname);

  setHostname();
}

//////////////////////////////////////////

AsyncESP32_W5500_Manager::~AsyncESP32_W5500_Manager()
{
#if USE_DYNAMIC_PARAMS

  if (_params != NULL)
  {
    LOGINFO(F("freeing allocated params!"));

    free(_params);
  }

#endif
}

//////////////////////////////////////////

#if USE_DYNAMIC_PARAMS
  bool AsyncESP32_W5500_Manager::addParameter(ESPAsync_EMParameter *p)
#else
  void AsyncESP32_W5500_Manager::addParameter(ESPAsync_EMParameter *p)
#endif
{
#if USE_DYNAMIC_PARAMS

  if (_paramsCount == _max_params)
  {
    // rezise the params array
    _max_params += ETH_MANAGER_MAX_PARAMS;

    LOGINFO1(F("Increasing _max_params to:"), _max_params);

    ESPAsync_EMParameter** new_params = (ESPAsync_EMParameter**)realloc(_params,
                                                                        _max_params * sizeof(ESPAsync_EMParameter*));

    if (new_params != NULL)
    {
      _params = new_params;
    }
    else
    {
      LOGINFO(F("ERROR: failed to realloc params, size not increased!"));

      return false;
    }
  }

  _params[_paramsCount] = p;
  _paramsCount++;

  LOGINFO1(F("Adding parameter"), p->getID());

  return true;

#else

  // Danger here. Better to use Tzapu way here
  if (_paramsCount < (ETH_MANAGER_MAX_PARAMS))
  {
    _params[_paramsCount] = p;
    _paramsCount++;

    LOGINFO1(F("Adding parameter"), p->getID());
  }
  else
  {
    LOGINFO("Can't add parameter. Full");
  }

#endif
}

//////////////////////////////////////////

void AsyncESP32_W5500_Manager::setupConfigPortal()
{
  stopConfigPortal = false; //Signal not to close config portal

#if !( USING_ESP32_S2 || USING_ESP32_C3 )
  server->reset();

  if (!dnsServer)
    dnsServer = new AsyncDNSServer;

#endif    // ( USING_ESP32_S2 || USING_ESP32_C3 )

  /* Setup the DNS server redirecting all the domains to the apIP */
  if (dnsServer)
  {
    dnsServer->setErrorReplyCode(AsyncDNSReplyCode::NoError);

    // AsyncDNSServer started with "*" domain name, all DNS requests will be passsed to ETH.localIP()
    if (! dnsServer->start(DNS_PORT, "*", ETH.localIP()))
    {
      // No socket available
      LOGERROR(F("Can't start DNS Server. No available socket"));
    }
  }

  _configPortalStart = millis();

  LOGDEBUG1(F("_configPortalStart millis() ="), millis());

  LOGWARN1(F("Config Portal IP address ="), ETH.localIP());

  /* Setup web pages: root, eth config pages, SO captive portal detectors and not found. */

  server->on("/",         std::bind(&AsyncESP32_W5500_Manager::handleRoot,        this,
                                    std::placeholders::_1)).setFilter(ON_AP_FILTER);
  server->on("/eth",     std::bind(&AsyncESP32_W5500_Manager::handleETH,        this,
                                   std::placeholders::_1)).setFilter(ON_AP_FILTER);
  server->on("/ethsave", std::bind(&AsyncESP32_W5500_Manager::handleETHSave,    this,
                                   std::placeholders::_1)).setFilter(ON_AP_FILTER);
  server->on("/close",    std::bind(&AsyncESP32_W5500_Manager::handleServerClose, this,
                                    std::placeholders::_1)).setFilter(ON_AP_FILTER);
  server->on("/i",        std::bind(&AsyncESP32_W5500_Manager::handleInfo,        this,
                                    std::placeholders::_1)).setFilter(ON_AP_FILTER);
  server->on("/r",        std::bind(&AsyncESP32_W5500_Manager::handleReset,       this,
                                    std::placeholders::_1)).setFilter(ON_AP_FILTER);
  server->on("/state",    std::bind(&AsyncESP32_W5500_Manager::handleState,       this,
                                    std::placeholders::_1)).setFilter(ON_AP_FILTER);
  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server->on("/fwlink",   std::bind(&AsyncESP32_W5500_Manager::handleRoot,        this,
                                    std::placeholders::_1)).setFilter(ON_AP_FILTER);
  server->onNotFound (std::bind(&AsyncESP32_W5500_Manager::handleNotFound,        this, std::placeholders::_1));

  server->begin(); // Web server start

  LOGWARN(F("HTTP server started"));
}

//////////////////////////////////////////

bool AsyncESP32_W5500_Manager::startConfigPortal()
{
  connect = false;

  setupConfigPortal();

  bool TimedOut = true;

  LOGINFO("startConfigPortal : Enter loop");

  while (true)
  {
    if (connect)
    {
      TimedOut = false;

      if (_shouldBreakAfterConfig)
      {
        //flag set to exit after config after trying to connect
        //notify that configuration has changed and any optional parameters should be saved
        if (_savecallback != NULL)
        {
          //todo: check if any custom parameters actually exist, and check if they really changed maybe
          _savecallback();
        }

        LOGDEBUG("Stop ConfigPortal: _shouldBreakAfterConfig");

        break;
      }
    }

    if (stopConfigPortal)
    {
      TimedOut = false;

      LOGERROR("stopConfigPortal");

      stopConfigPortal = false;

      break;
    }

    if (_configPortalTimeout > 0 && ( millis() > _configPortalStart + _configPortalTimeout) )
    {
      //LOGDEBUG3("startConfigPortal: timeout, _configPortalTimeout =", _configPortalTimeout, "millis() =", millis());

      stopConfigPortal = false;

      break;
    }

#define TIME_BETWEEN_CONFIG_PORTAL_LOOP       50

    vTaskDelay(TIME_BETWEEN_CONFIG_PORTAL_LOOP / portTICK_PERIOD_MS);
  }

  //LOGDEBUG3("startConfigPortal: exit, _configPortalTimeout =", _configPortalTimeout, "millis() =", millis());

#if !( USING_ESP32_S2 || USING_ESP32_C3 )
  server->reset();
  dnsServer->stop();
#endif

  return  (ESP32_W5500_isConnected());
}

//////////////////////////////////////////

void AsyncESP32_W5500_Manager::setTimeout(const unsigned long& seconds)
{
  setConfigPortalTimeout(seconds);
}

//////////////////////////////////////////

void AsyncESP32_W5500_Manager::setConfigPortalTimeout(const unsigned long& seconds)
{
  _configPortalTimeout = seconds * 1000;
}

//////////////////////////////////////////

void AsyncESP32_W5500_Manager::setConnectTimeout(const unsigned long& seconds)
{
  _connectTimeout = seconds * 1000;
}

void AsyncESP32_W5500_Manager::setDebugOutput(bool debug)
{
  _debug = debug;
}

//////////////////////////////////////////

void AsyncESP32_W5500_Manager::setBreakAfterConfig(bool shouldBreak)
{
  _shouldBreakAfterConfig = shouldBreak;
}

//////////////////////////////////////////

void AsyncESP32_W5500_Manager::reportStatus(String& page)
{
  page += FPSTR(EM_HTTP_SCRIPT_NTP_MSG);
}



//////////////////////////////////////////

void AsyncESP32_W5500_Manager::setSTAStaticIPConfig(const IPAddress& ip, const IPAddress& gw, const IPAddress& sn)
{
  LOGINFO(F("setSTAStaticIPConfig"));

  _ETH_STA_IPconfig._sta_static_ip = ip;
  _ETH_STA_IPconfig._sta_static_gw = gw;
  _ETH_STA_IPconfig._sta_static_sn = sn;
}

//////////////////////////////////////////

void AsyncESP32_W5500_Manager::setSTAStaticIPConfig(const ETH_STA_IPConfig& EM_STA_IPconfig)
{
  LOGINFO(F("setSTAStaticIPConfig"));

  memcpy((void *) &_ETH_STA_IPconfig, &EM_STA_IPconfig, sizeof(_ETH_STA_IPconfig));
}

//////////////////////////////////////////

void AsyncESP32_W5500_Manager::getSTAStaticIPConfig(ETH_STA_IPConfig& EM_STA_IPconfig)
{
  LOGINFO(F("getSTAStaticIPConfig"));

  memcpy((void *) &EM_STA_IPconfig, &_ETH_STA_IPconfig, sizeof(EM_STA_IPconfig));
}


//////////////////////////////////////////

#if USE_CONFIGURABLE_DNS
void AsyncESP32_W5500_Manager::setSTAStaticIPConfig(const IPAddress& ip, const IPAddress& gw, const IPAddress& sn,
                                                    const IPAddress& dns_address_1, const IPAddress& dns_address_2)
{
  LOGINFO(F("setSTAStaticIPConfig for USE_CONFIGURABLE_DNS"));

  _ETH_STA_IPconfig._sta_static_ip = ip;
  _ETH_STA_IPconfig._sta_static_gw = gw;
  _ETH_STA_IPconfig._sta_static_sn = sn;
  _ETH_STA_IPconfig._sta_static_dns1 = dns_address_1; //***** Added argument *****
  _ETH_STA_IPconfig._sta_static_dns2 = dns_address_2; //***** Added argument *****
}
#endif

//////////////////////////////////////////

// Handle root or redirect to captive portal
void AsyncESP32_W5500_Manager::handleRoot(AsyncWebServerRequest *request)
{
  LOGDEBUG(F("handleRoot"));

  // Disable _configPortalTimeout when someone accessing Portal to give some time to config
  _configPortalTimeout = 0;

  if (captivePortal(request))
  {
    LOGDEBUG(F("handleRoot: captive portal exit"));

    // If captive portal redirect instead of displaying the error page.
    return;
  }

  String page = FPSTR(EM_HTTP_HEAD_START);
  page.replace("{v}", "Options");

  page += FPSTR(EM_HTTP_SCRIPT);
  page += FPSTR(EM_HTTP_SCRIPT_NTP);
  page += FPSTR(EM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(EM_HTTP_HEAD_END);

  page += FPSTR(EM_FLDSET_START);
  page += FPSTR(EM_HTTP_PORTAL_OPTIONS);
  page += F("<div class=\"msg\">");

  reportStatus(page);

  page += F("</div>");
  page += FPSTR(EM_FLDSET_END);
  page += FPSTR(EM_HTTP_END);

#if ( USING_ESP32_S2 || USING_ESP32_C3 )
  request->send(200, EM_HTTP_HEAD_CT, page);

  // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
  delay(1);
#else

  AsyncWebServerResponse *response = request->beginResponse(200, EM_HTTP_HEAD_CT, page);
  response->addHeader(FPSTR(EM_HTTP_CACHE_CONTROL), FPSTR(EM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
  // For configuring CORS Header, default to EM_HTTP_CORS_ALLOW_ALL = "*"
  response->addHeader(FPSTR(EM_HTTP_CORS), _CORS_Header);
#endif

  response->addHeader(FPSTR(EM_HTTP_PRAGMA), FPSTR(EM_HTTP_NO_CACHE));
  response->addHeader(FPSTR(EM_HTTP_EXPIRES), "-1");

  request->send(response);

#endif    // ( USING_ESP32_S2 || USING_ESP32_C3 )
}

//////////////////////////////////////////

// ETH config page handler
void AsyncESP32_W5500_Manager::handleETH(AsyncWebServerRequest *request)
{
  LOGDEBUG(F("Handle ETH"));

  // Disable _configPortalTimeout when someone accessing Portal to give some time to config
  _configPortalTimeout = 0;

  String page = FPSTR(EM_HTTP_HEAD_START);
  page.replace("{v}", "Config ESP");

  page += FPSTR(EM_HTTP_SCRIPT);
  page += FPSTR(EM_HTTP_SCRIPT_NTP);
  page += FPSTR(EM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(EM_HTTP_HEAD_END);
  page += F("<h2>Configuration</h2>");

  page += FPSTR(EM_HTTP_FORM_START);

  char parLength[2];

  page += FPSTR(EM_FLDSET_START);

  // add the extra parameters to the form
  for (int i = 0; i < _paramsCount; i++)
  {
    if (_params[i] == NULL)
    {
      break;
    }

    String pitem;

    switch (_params[i]->getLabelPlacement())
    {
      case WFM_LABEL_BEFORE:
        pitem = FPSTR(EM_HTTP_FORM_LABEL_BEFORE);
        break;

      case WFM_LABEL_AFTER:
        pitem = FPSTR(EM_HTTP_FORM_LABEL_AFTER);
        break;

      default:
        // WFM_NO_LABEL
        pitem = FPSTR(EM_HTTP_FORM_PARAM);
        break;
    }

    if (_params[i]->getID() != NULL)
    {
      pitem.replace("{i}", _params[i]->getID());
      pitem.replace("{n}", _params[i]->getID());
      pitem.replace("{p}", _params[i]->getPlaceholder());

      snprintf(parLength, 2, "%d", _params[i]->getValueLength());

      pitem.replace("{l}", parLength);
      pitem.replace("{v}", _params[i]->getValue());
      pitem.replace("{c}", _params[i]->getCustomHTML());
    }
    else
    {
      pitem = _params[i]->getCustomHTML();
    }

    page += pitem;
  }

  if (_paramsCount > 0)
  {
    page += FPSTR(EM_FLDSET_END);
  }

  if (_params[0] != NULL)
  {
    page += "<br/>";
  }

  LOGDEBUG1(F("Static IP ="), _ETH_STA_IPconfig._sta_static_ip.toString());

  // KH, Comment out to permit changing from DHCP to static IP, or vice versa
  // and add staticIP label in CP

  // To permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
  // You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
  // You have to explicitly specify false to disable the feature.

#if !USE_STATIC_IP_CONFIG_IN_CP

  if (_ETH_STA_IPconfig._sta_static_ip)
#endif
  {
    page += FPSTR(EM_FLDSET_START);

    String item = FPSTR(EM_HTTP_FORM_LABEL);

    item += FPSTR(EM_HTTP_FORM_PARAM);

    item.replace("{i}", "ip");
    item.replace("{n}", "ip");
    item.replace("{p}", "Static IP");
    item.replace("{l}", "15");
    item.replace("{v}", _ETH_STA_IPconfig._sta_static_ip.toString());

    page += item;

    item = FPSTR(EM_HTTP_FORM_LABEL);
    item += FPSTR(EM_HTTP_FORM_PARAM);

    item.replace("{i}", "gw");
    item.replace("{n}", "gw");
    item.replace("{p}", "Gateway IP");
    item.replace("{l}", "15");
    item.replace("{v}", _ETH_STA_IPconfig._sta_static_gw.toString());

    page += item;

    item = FPSTR(EM_HTTP_FORM_LABEL);
    item += FPSTR(EM_HTTP_FORM_PARAM);

    item.replace("{i}", "sn");
    item.replace("{n}", "sn");
    item.replace("{p}", "Subnet");
    item.replace("{l}", "15");
    item.replace("{v}", _ETH_STA_IPconfig._sta_static_sn.toString());

#if USE_CONFIGURABLE_DNS
    //***** Added for DNS address options *****
    page += item;

    item = FPSTR(EM_HTTP_FORM_LABEL);
    item += FPSTR(EM_HTTP_FORM_PARAM);

    item.replace("{i}", "dns1");
    item.replace("{n}", "dns1");
    item.replace("{p}", "DNS1 IP");
    item.replace("{l}", "15");
    item.replace("{v}", _ETH_STA_IPconfig._sta_static_dns1.toString());

    page += item;

    item = FPSTR(EM_HTTP_FORM_LABEL);
    item += FPSTR(EM_HTTP_FORM_PARAM);

    item.replace("{i}", "dns2");
    item.replace("{n}", "dns2");
    item.replace("{p}", "DNS2 IP");
    item.replace("{l}", "15");
    item.replace("{v}", _ETH_STA_IPconfig._sta_static_dns2.toString());
    //***** End added for DNS address options *****
#endif

    page += item;
    page += FPSTR(EM_FLDSET_END);
    page += "<br/>";
  }

  page += FPSTR(EM_HTTP_SCRIPT_NTP_HIDDEN);
  page += FPSTR(EM_HTTP_FORM_END);
  page += FPSTR(EM_HTTP_END);

#if ( USING_ESP32_S2 || USING_ESP32_C3 )
  request->send(200, EM_HTTP_HEAD_CT, page);

  // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
  delay(1);
#else

  AsyncWebServerResponse *response = request->beginResponse(200, EM_HTTP_HEAD_CT, page);

  response->addHeader(FPSTR(EM_HTTP_CACHE_CONTROL), FPSTR(EM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
  response->addHeader(FPSTR(EM_HTTP_CORS), _CORS_Header);
#endif

  response->addHeader(FPSTR(EM_HTTP_PRAGMA), FPSTR(EM_HTTP_NO_CACHE));
  response->addHeader(FPSTR(EM_HTTP_EXPIRES), "-1");

  request->send(response);

#endif    // ( USING_ESP32_S2 || USING_ESP32_C3 )

  LOGDEBUG(F("Sent config page"));
}

//////////////////////////////////////////

// Handle the WLAN save form and redirect to WLAN config page again
void AsyncESP32_W5500_Manager::handleETHSave(AsyncWebServerRequest *request)
{
  LOGDEBUG(F("ETH save"));

#if USE_ESP_ETH_MANAGER_NTP

  if (request->hasArg("timezone"))
  {
    _timezoneName = request->arg("timezone");   //.c_str();

    LOGDEBUG1(F("TZ ="), _timezoneName);
  }
  else
  {
    LOGDEBUG(F("No TZ arg"));
  }

#endif

  ///////////////////////

  //parameters
  for (int i = 0; i < _paramsCount; i++)
  {
    if (_params[i] == NULL)
    {
      break;
    }

    //read parameter
    String value = request->arg(_params[i]->getID()).c_str();

    //store it in array
    value.toCharArray(_params[i]->_WMParam_data._value, _params[i]->_WMParam_data._length);

    LOGDEBUG2(F("Parameter and value :"), _params[i]->getID(), value);
  }


  if (request->hasArg("ip"))
  {
    String ip = request->arg("ip");

    optionalIPFromString(&_ETH_STA_IPconfig._sta_static_ip, ip.c_str());

    LOGDEBUG1(F("New Static IP ="), _ETH_STA_IPconfig._sta_static_ip.toString());
  }

  if (request->hasArg("gw"))
  {
    String gw = request->arg("gw");

    optionalIPFromString(&_ETH_STA_IPconfig._sta_static_gw, gw.c_str());

    LOGDEBUG1(F("New Static Gateway ="), _ETH_STA_IPconfig._sta_static_gw.toString());
  }

  if (request->hasArg("sn"))
  {
    String sn = request->arg("sn");

    optionalIPFromString(&_ETH_STA_IPconfig._sta_static_sn, sn.c_str());

    LOGDEBUG1(F("New Static Netmask ="), _ETH_STA_IPconfig._sta_static_sn.toString());
  }

#if USE_CONFIGURABLE_DNS

  //*****  Added for DNS Options *****
  if (request->hasArg("dns1"))
  {
    String dns1 = request->arg("dns1");

    optionalIPFromString(&_ETH_STA_IPconfig._sta_static_dns1, dns1.c_str());

    LOGDEBUG1(F("New Static DNS1 ="), _ETH_STA_IPconfig._sta_static_dns1.toString());
  }

  if (request->hasArg("dns2"))
  {
    String dns2 = request->arg("dns2");

    optionalIPFromString(&_ETH_STA_IPconfig._sta_static_dns2, dns2.c_str());

    LOGDEBUG1(F("New Static DNS2 ="), _ETH_STA_IPconfig._sta_static_dns2.toString());
  }

  //*****  End added for DNS Options *****
#endif

  String page = FPSTR(EM_HTTP_HEAD_START);
  page.replace("{v}", "Credentials Saved");

  page += FPSTR(EM_HTTP_SCRIPT);
  page += FPSTR(EM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(EM_HTTP_HEAD_END);
  page += FPSTR(EM_HTTP_SAVED);

  page += FPSTR(EM_HTTP_END);

#if ( USING_ESP32_S2 || USING_ESP32_C3 )
  request->send(200, EM_HTTP_HEAD_CT, page);

  // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
  delay(1);
#else

  AsyncWebServerResponse *response = request->beginResponse(200, EM_HTTP_HEAD_CT, page);
  response->addHeader(FPSTR(EM_HTTP_CACHE_CONTROL), FPSTR(EM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
  response->addHeader(FPSTR(EM_HTTP_CORS), _CORS_Header);
#endif

  response->addHeader(FPSTR(EM_HTTP_PRAGMA), FPSTR(EM_HTTP_NO_CACHE));
  response->addHeader(FPSTR(EM_HTTP_EXPIRES), "-1");

  request->send(response);

#endif    // ( USING_ESP32_S2 || USING_ESP32_C3 )

  LOGDEBUG(F("Sent eth save page"));

  connect = true; //signal ready to connect/reset

  stopConfigPortal = true; //signal ready to shutdown config portal
}

//////////////////////////////////////////

// Handle shut down the server page
void AsyncESP32_W5500_Manager::handleServerClose(AsyncWebServerRequest *request)
{
  LOGDEBUG(F("Server Close"));

  String page = FPSTR(EM_HTTP_HEAD_START);
  page.replace("{v}", "Close Server");

  page += FPSTR(EM_HTTP_SCRIPT);
  page += FPSTR(EM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(EM_HTTP_HEAD_END);
  page += F("<div class=\"msg\">");
  page += F("</b><br>");
  page += F("IP address is <b>");
  page += ETH.localIP().toString();
  page += F("</b><br><br>");
  page += F("Portal closed...<br><br>");

  //page += F("Push button on device to restart configuration server!");

  page += FPSTR(EM_HTTP_END);

#if ( USING_ESP32_S2 || USING_ESP32_C3 )
  request->send(200, EM_HTTP_HEAD_CT, page);

  // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
  delay(1);
#else

  AsyncWebServerResponse *response = request->beginResponse(200, EM_HTTP_HEAD_CT, page);
  response->addHeader(FPSTR(EM_HTTP_CACHE_CONTROL), FPSTR(EM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
  response->addHeader(FPSTR(EM_HTTP_CORS), _CORS_Header);
#endif

  response->addHeader(FPSTR(EM_HTTP_PRAGMA), FPSTR(EM_HTTP_NO_CACHE));
  response->addHeader(FPSTR(EM_HTTP_EXPIRES), "-1");

  request->send(response);

#endif    // ( USING_ESP32_S2 || USING_ESP32_C3 )

  stopConfigPortal = true; //signal ready to shutdown config portal

  LOGDEBUG(F("Sent server close page"));
}

//////////////////////////////////////////

// Handle the info page
void AsyncESP32_W5500_Manager::handleInfo(AsyncWebServerRequest *request)
{
  LOGDEBUG(F("Info"));

  // Disable _configPortalTimeout when someone accessing Portal to give some time to config
  _configPortalTimeout = 0;

  String page = FPSTR(EM_HTTP_HEAD_START);
  page.replace("{v}", "Info");

  page += FPSTR(EM_HTTP_SCRIPT);
  page += FPSTR(EM_HTTP_SCRIPT_NTP);
  page += FPSTR(EM_HTTP_STYLE);
  page += _customHeadElement;

  if (connect)
    page += F("<meta http-equiv=\"refresh\" content=\"5; url=/i\">");

  page += FPSTR(EM_HTTP_HEAD_END);

  page += F("<dl>");

  if (connect)
  {
    page += F("<dt>Trying to connect</dt><dd>");
    page += ethStatus;
    page += F("</dd>");
  }

  page += pager;
  page += F("<h2>Information</h2>");

  reportStatus(page);

  page += FPSTR(EM_FLDSET_START);
  page += F("<h3>Device Data</h3>");
  page += F("<table class=\"table\">");
  page += F("<thead><tr><th>Name</th><th>Value</th></tr></thead><tbody><tr><td>Chip ID</td><td>");

#ifdef ESP8266
  page += String(ESP.getChipId(), HEX);
#else   //ESP32

  page += String(ESP_getChipId(), HEX);
  page += F("</td></tr>");

  page += F("<tr><td>Chip OUI</td><td>");
  page += F("0x");
  page += String(getChipOUI(), HEX);
  page += F("</td></tr>");

  page += F("<tr><td>Chip Model</td><td>");
  page += ESP.getChipModel();
  page += F(" Rev");
  page += ESP.getChipRevision();
#endif

  page += F("</td></tr>");

  page += F("<tr><td>Flash Chip ID</td><td>");

#ifdef ESP8266
  page += String(ESP.getFlashChipId(), HEX);
#else   //ESP32
  // TODO
  page += F("TODO");
#endif

  page += F("</td></tr>");

  page += F("<tr><td>IDE Flash Size</td><td>");
  page += ESP.getFlashChipSize();
  page += F(" bytes</td></tr>");

  page += F("<tr><td>Real Flash Size</td><td>");

#ifdef ESP8266
  page += ESP.getFlashChipRealSize();
#else   //ESP32
  // TODO
  page += F("TODO");
#endif

  page += F(" bytes</td></tr>");

  page += F("<tr><td>Station IP</td><td>");
  page += ETH.localIP().toString();
  page += F("</td></tr>");

  page += F("<tr><td>Station MAC</td><td>");
  page += ETH.macAddress();
  page += F("</td></tr>");
  page += F("</tbody></table>");

  page += FPSTR(EM_FLDSET_END);

#if USE_AVAILABLE_PAGES
  page += FPSTR(EM_FLDSET_START);
  page += FPSTR(EM_HTTP_AVAILABLE_PAGES);
  page += FPSTR(EM_FLDSET_END);
#endif

  page += F("<p/>More information about AsyncESP32_W5500_Manager at");
  page += F("<p/><a href=\"https://github.com/khoih-prog/AsyncESP32_W5500_Manager\">https://github.com/khoih-prog/AsyncESP32_W5500_Manager</a>");
  page += FPSTR(EM_HTTP_END);

#if ( USING_ESP32_S2 || USING_ESP32_C3 )
  request->send(200, EM_HTTP_HEAD_CT, page);

  // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
  delay(1);
#else

  AsyncWebServerResponse *response = request->beginResponse(200, EM_HTTP_HEAD_CT, page);

  response->addHeader(FPSTR(EM_HTTP_CACHE_CONTROL), FPSTR(EM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
  response->addHeader(FPSTR(EM_HTTP_CORS), _CORS_Header);
#endif

  response->addHeader(FPSTR(EM_HTTP_PRAGMA), FPSTR(EM_HTTP_NO_CACHE));
  response->addHeader(FPSTR(EM_HTTP_EXPIRES), "-1");

  request->send(response);

#endif    // ( USING_ESP32_S2 || USING_ESP32_C3 )

  LOGDEBUG(F("Info page sent"));
}

//////////////////////////////////////////

// Handle the state page
void AsyncESP32_W5500_Manager::handleState(AsyncWebServerRequest *request)
{
  LOGDEBUG(F("State-Json"));

  String page = F("{\"Soft_AP_IP\":\"");

  page += F("\",\"Station_IP\":\"");
  page += ETH.localIP().toString();
  page += F("\",");

#if ( USING_ESP32_S2 || USING_ESP32_C3 )
  request->send(200, EM_HTTP_HEAD_CT, page);

  // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
  delay(1);
#else

  AsyncWebServerResponse *response = request->beginResponse(200, "application/json", page);
  response->addHeader(FPSTR(EM_HTTP_CACHE_CONTROL), FPSTR(EM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
  response->addHeader(FPSTR(EM_HTTP_CORS), _CORS_Header);
#endif

  response->addHeader(FPSTR(EM_HTTP_PRAGMA), FPSTR(EM_HTTP_NO_CACHE));
  response->addHeader(FPSTR(EM_HTTP_EXPIRES), "-1");

  request->send(response);
#endif    // ( USING_ESP32_S2 || USING_ESP32_C3 )

  LOGDEBUG(F("Sent state page in json format"));
}

//////////////////////////////////////////

// Handle the reset page
void AsyncESP32_W5500_Manager::handleReset(AsyncWebServerRequest *request)
{
  LOGDEBUG(F("Reset"));

  String page = FPSTR(EM_HTTP_HEAD_START);
  page.replace("{v}", "Information");

  page += FPSTR(EM_HTTP_SCRIPT);
  page += FPSTR(EM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(EM_HTTP_HEAD_END);
  page += F("Resetting");
  page += FPSTR(EM_HTTP_END);

#if ( USING_ESP32_S2 || USING_ESP32_C3 )
  request->send(200, EM_HTTP_HEAD_CT, page);
#else

  AsyncWebServerResponse *response = request->beginResponse(200, EM_HTTP_HEAD_CT, page);

  response->addHeader(EM_HTTP_CACHE_CONTROL, EM_HTTP_NO_STORE);
  response->addHeader(EM_HTTP_PRAGMA, EM_HTTP_NO_CACHE);
  response->addHeader(EM_HTTP_EXPIRES, "-1");

  request->send(response);
#endif    // ( USING_ESP32_S2 || USING_ESP32_C3 )

  LOGDEBUG(F("Sent reset page"));
  delay(5000);

  ESP.restart();

  delay(2000);
}

//////////////////////////////////////////

void AsyncESP32_W5500_Manager::handleNotFound(AsyncWebServerRequest *request)
{
  if (captivePortal(request))
  {
    LOGDEBUG(F("handleNotFound: captive portal exit"));

    // If captive portal redirect instead of displaying the error page.
    return;
  }

  String message = "File Not Found\n\n";

  message += "URI: ";
  message += request->url();
  message += "\nMethod: ";
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += request->args();
  message += "\n";

  for (uint8_t i = 0; i < request->args(); i++)
  {
    message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
  }

#if ( USING_ESP32_S2 || USING_ESP32_C3 )
  request->send(200, EM_HTTP_HEAD_CT, message);

  // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
  delay(1);
#else

  AsyncWebServerResponse *response = request->beginResponse( 404, EM_HTTP_HEAD_CT2, message );

  response->addHeader(FPSTR(EM_HTTP_CACHE_CONTROL), FPSTR(EM_HTTP_NO_STORE));
  response->addHeader(FPSTR(EM_HTTP_PRAGMA), FPSTR(EM_HTTP_NO_CACHE));
  response->addHeader(FPSTR(EM_HTTP_EXPIRES), "-1");

  request->send(response);
#endif    // ( USING_ESP32_S2 || USING_ESP32_C3 )
}

//////////////////////////////////////////

/**
   HTTPD redirector
   Redirect to captive portal if we got a request for another domain.
   Return true in that case so the page handler do not try to handle the request again.
*/
bool AsyncESP32_W5500_Manager::captivePortal(AsyncWebServerRequest *request)
{
  if (!isIp(request->host()))
  {
    LOGINFO(F("Request redirected to captive portal"));
    LOGINFO1(F("Location http://"), toStringIp(request->client()->localIP()));

    AsyncWebServerResponse *response = request->beginResponse(302, EM_HTTP_HEAD_CT2, "");

    response->addHeader("Location", String("http://") + toStringIp(request->client()->localIP()));

    request->send(response);

    return true;
  }

  LOGDEBUG1(F("request host IP ="), request->host());

  return false;
}

//////////////////////////////////////////

// start up save config callback
void AsyncESP32_W5500_Manager::setSaveConfigCallback(void(*func)())
{
  _savecallback = func;
}

//////////////////////////////////////////

// sets a custom element to add to head, like a new style tag
void AsyncESP32_W5500_Manager::setCustomHeadElement(const char* element)
{
  _customHeadElement = element;
}

//////////////////////////////////////////

// Is this an IP?
bool AsyncESP32_W5500_Manager::isIp(const String& str)
{
  for (unsigned int i = 0; i < str.length(); i++)
  {
    int c = str.charAt(i);

    if ( (c != '.') && (c != ':') && ( (c < '0') || (c > '9') ) )
    {
      return false;
    }
  }

  return true;
}

//////////////////////////////////////////

// IP to String
String AsyncESP32_W5500_Manager::toStringIp(const IPAddress& ip)
{
  String res = "";

  for (int i = 0; i < 3; i++)
  {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }

  res += String(((ip >> 8 * 3)) & 0xFF);

  return res;
}

//////////////////////////////////////////

uint32_t getChipID()
{
  uint64_t chipId64 = 0;

  for (int i = 0; i < 6; i++)
  {
    chipId64 |= ( ( (uint64_t) ESP.getEfuseMac() >> (40 - (i * 8)) ) & 0xff ) << (i * 8);
  }

  return (uint32_t) (chipId64 & 0xFFFFFF);
}

//////////////////////////////////////////

uint32_t getChipOUI()
{
  uint64_t chipId64 = 0;

  for (int i = 0; i < 6; i++)
  {
    chipId64 |= ( ( (uint64_t) ESP.getEfuseMac() >> (40 - (i * 8)) ) & 0xff ) << (i * 8);
  }

  return (uint32_t) (chipId64 >> 24);
}

//////////////////////////////////////////

#endif    // AsyncESP32_W5500_Manager_Impl_h
