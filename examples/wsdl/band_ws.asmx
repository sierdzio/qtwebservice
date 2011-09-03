<?xml version="1.0" encoding="utf-8"?>
<wsdl:definitions xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/" xmlns:tm="http://microsoft.com/wsdl/mime/textMatching/" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/" xmlns:mime="http://schemas.xmlsoap.org/wsdl/mime/" xmlns:tns="http://tempuri.org/" xmlns:s="http://www.w3.org/2001/XMLSchema" xmlns:soap12="http://schemas.xmlsoap.org/wsdl/soap12/" xmlns:http="http://schemas.xmlsoap.org/wsdl/http/" targetNamespace="http://tempuri.org/" xmlns:wsdl="http://schemas.xmlsoap.org/wsdl/">
  <wsdl:types>
    <s:schema elementFormDefault="qualified" targetNamespace="http://tempuri.org/">
      <s:element name="getBandName">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="1" maxOccurs="1" name="bandId" type="s:int" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="getBandNameResponse">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="0" maxOccurs="1" name="getBandNameResult" type="s:string" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="getBandDescription">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="1" maxOccurs="1" name="bandId" type="s:int" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="getBandDescriptionResponse">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="0" maxOccurs="1" name="getBandDescriptionResult" type="tns:ArrayOfString" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:complexType name="ArrayOfString">
        <s:sequence>
          <s:element minOccurs="0" maxOccurs="unbounded" name="string" nillable="true" type="s:string" />
        </s:sequence>
      </s:complexType>
      <s:element name="getBandsList">
        <s:complexType />
      </s:element>
      <s:element name="getBandsListResponse">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="0" maxOccurs="1" name="getBandsListResult" type="tns:ArrayOfString" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="getBandsListForGenreAndDate">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="0" maxOccurs="1" name="genreName" type="s:string" />
            <s:element minOccurs="1" maxOccurs="1" name="date" type="s:dateTime" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="getBandsListForGenreAndDateResponse">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="0" maxOccurs="1" name="getBandsListForGenreAndDateResult" type="tns:ArrayOfString" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="getBandsListForGenre">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="0" maxOccurs="1" name="genreName" type="s:string" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="getBandsListForGenreResponse">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="0" maxOccurs="1" name="getBandsListForGenreResult" type="tns:ArrayOfString" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="getGenreList">
        <s:complexType />
      </s:element>
      <s:element name="getGenreListResponse">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="0" maxOccurs="1" name="getGenreListResult" type="tns:ArrayOfString" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="getBandPricePerShow">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="1" maxOccurs="1" name="bandId" type="s:int" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="getBandPricePerShowResponse">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="1" maxOccurs="1" name="getBandPricePerShowResult" type="s:int" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="getBandsForADate">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="1" maxOccurs="1" name="date" type="s:dateTime" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="getBandsForADateResponse">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="0" maxOccurs="1" name="getBandsForADateResult" type="tns:ArrayOfString" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="getNextEmptySlot">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="1" maxOccurs="1" name="currentDate" type="s:dateTime" />
            <s:element minOccurs="1" maxOccurs="1" name="bandId" type="s:int" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="getNextEmptySlotResponse">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="1" maxOccurs="1" name="getNextEmptySlotResult" type="s:dateTime" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="bookABand">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="1" maxOccurs="1" name="from" type="s:dateTime" />
            <s:element minOccurs="1" maxOccurs="1" name="to" type="s:dateTime" />
            <s:element minOccurs="0" maxOccurs="1" name="bookingCode" type="s:string" />
            <s:element minOccurs="1" maxOccurs="1" name="bandID" type="s:int" />
            <s:element minOccurs="0" maxOccurs="1" name="customerName" type="s:string" />
            <s:element minOccurs="0" maxOccurs="1" name="customerSurname" type="s:string" />
            <s:element minOccurs="0" maxOccurs="1" name="Phone" type="s:string" />
            <s:element minOccurs="0" maxOccurs="1" name="customerEmail" type="s:string" />
            <s:element minOccurs="0" maxOccurs="1" name="customerAddress" type="s:string" />
            <s:element minOccurs="0" maxOccurs="1" name="receptionAddress" type="s:string" />
            <s:element minOccurs="1" maxOccurs="1" name="numberOfGuests" type="s:int" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="bookABandResponse">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="1" maxOccurs="1" name="bookABandResult" type="s:boolean" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="cancelBookingById">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="1" maxOccurs="1" name="bookingID" type="s:int" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="cancelBookingByIdResponse">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="1" maxOccurs="1" name="cancelBookingByIdResult" type="s:boolean" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="cancelBooking">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="1" maxOccurs="1" name="bandId" type="s:int" />
            <s:element minOccurs="1" maxOccurs="1" name="dateFrom" type="s:dateTime" />
            <s:element minOccurs="1" maxOccurs="1" name="dateTo" type="s:dateTime" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="cancelBookingResponse">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="1" maxOccurs="1" name="cancelBookingResult" type="s:boolean" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="getBandSchedule">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="0" maxOccurs="1" name="bandName" type="s:string" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:element name="getBandScheduleResponse">
        <s:complexType>
          <s:sequence>
            <s:element minOccurs="0" maxOccurs="1" name="getBandScheduleResult" type="tns:ArrayOfDateTime" />
          </s:sequence>
        </s:complexType>
      </s:element>
      <s:complexType name="ArrayOfDateTime">
        <s:sequence>
          <s:element minOccurs="0" maxOccurs="unbounded" name="dateTime" type="s:dateTime" />
        </s:sequence>
      </s:complexType>
    </s:schema>
  </wsdl:types>
  <wsdl:message name="getBandNameSoapIn">
    <wsdl:part name="parameters" element="tns:getBandName" />
  </wsdl:message>
  <wsdl:message name="getBandNameSoapOut">
    <wsdl:part name="parameters" element="tns:getBandNameResponse" />
  </wsdl:message>
  <wsdl:message name="getBandDescriptionSoapIn">
    <wsdl:part name="parameters" element="tns:getBandDescription" />
  </wsdl:message>
  <wsdl:message name="getBandDescriptionSoapOut">
    <wsdl:part name="parameters" element="tns:getBandDescriptionResponse" />
  </wsdl:message>
  <wsdl:message name="getBandsListSoapIn">
    <wsdl:part name="parameters" element="tns:getBandsList" />
  </wsdl:message>
  <wsdl:message name="getBandsListSoapOut">
    <wsdl:part name="parameters" element="tns:getBandsListResponse" />
  </wsdl:message>
  <wsdl:message name="getBandsListForGenreAndDateSoapIn">
    <wsdl:part name="parameters" element="tns:getBandsListForGenreAndDate" />
  </wsdl:message>
  <wsdl:message name="getBandsListForGenreAndDateSoapOut">
    <wsdl:part name="parameters" element="tns:getBandsListForGenreAndDateResponse" />
  </wsdl:message>
  <wsdl:message name="getBandsListForGenreSoapIn">
    <wsdl:part name="parameters" element="tns:getBandsListForGenre" />
  </wsdl:message>
  <wsdl:message name="getBandsListForGenreSoapOut">
    <wsdl:part name="parameters" element="tns:getBandsListForGenreResponse" />
  </wsdl:message>
  <wsdl:message name="getGenreListSoapIn">
    <wsdl:part name="parameters" element="tns:getGenreList" />
  </wsdl:message>
  <wsdl:message name="getGenreListSoapOut">
    <wsdl:part name="parameters" element="tns:getGenreListResponse" />
  </wsdl:message>
  <wsdl:message name="getBandPricePerShowSoapIn">
    <wsdl:part name="parameters" element="tns:getBandPricePerShow" />
  </wsdl:message>
  <wsdl:message name="getBandPricePerShowSoapOut">
    <wsdl:part name="parameters" element="tns:getBandPricePerShowResponse" />
  </wsdl:message>
  <wsdl:message name="getBandsForADateSoapIn">
    <wsdl:part name="parameters" element="tns:getBandsForADate" />
  </wsdl:message>
  <wsdl:message name="getBandsForADateSoapOut">
    <wsdl:part name="parameters" element="tns:getBandsForADateResponse" />
  </wsdl:message>
  <wsdl:message name="getNextEmptySlotSoapIn">
    <wsdl:part name="parameters" element="tns:getNextEmptySlot" />
  </wsdl:message>
  <wsdl:message name="getNextEmptySlotSoapOut">
    <wsdl:part name="parameters" element="tns:getNextEmptySlotResponse" />
  </wsdl:message>
  <wsdl:message name="bookABandSoapIn">
    <wsdl:part name="parameters" element="tns:bookABand" />
  </wsdl:message>
  <wsdl:message name="bookABandSoapOut">
    <wsdl:part name="parameters" element="tns:bookABandResponse" />
  </wsdl:message>
  <wsdl:message name="cancelBookingByIdSoapIn">
    <wsdl:part name="parameters" element="tns:cancelBookingById" />
  </wsdl:message>
  <wsdl:message name="cancelBookingByIdSoapOut">
    <wsdl:part name="parameters" element="tns:cancelBookingByIdResponse" />
  </wsdl:message>
  <wsdl:message name="cancelBookingSoapIn">
    <wsdl:part name="parameters" element="tns:cancelBooking" />
  </wsdl:message>
  <wsdl:message name="cancelBookingSoapOut">
    <wsdl:part name="parameters" element="tns:cancelBookingResponse" />
  </wsdl:message>
  <wsdl:message name="getBandScheduleSoapIn">
    <wsdl:part name="parameters" element="tns:getBandSchedule" />
  </wsdl:message>
  <wsdl:message name="getBandScheduleSoapOut">
    <wsdl:part name="parameters" element="tns:getBandScheduleResponse" />
  </wsdl:message>
  <wsdl:portType name="band_wsSoap">
    <wsdl:operation name="getBandName">
      <wsdl:input message="tns:getBandNameSoapIn" />
      <wsdl:output message="tns:getBandNameSoapOut" />
    </wsdl:operation>
    <wsdl:operation name="getBandDescription">
      <wsdl:input message="tns:getBandDescriptionSoapIn" />
      <wsdl:output message="tns:getBandDescriptionSoapOut" />
    </wsdl:operation>
    <wsdl:operation name="getBandsList">
      <wsdl:input message="tns:getBandsListSoapIn" />
      <wsdl:output message="tns:getBandsListSoapOut" />
    </wsdl:operation>
    <wsdl:operation name="getBandsListForGenreAndDate">
      <wsdl:input message="tns:getBandsListForGenreAndDateSoapIn" />
      <wsdl:output message="tns:getBandsListForGenreAndDateSoapOut" />
    </wsdl:operation>
    <wsdl:operation name="getBandsListForGenre">
      <wsdl:input message="tns:getBandsListForGenreSoapIn" />
      <wsdl:output message="tns:getBandsListForGenreSoapOut" />
    </wsdl:operation>
    <wsdl:operation name="getGenreList">
      <wsdl:input message="tns:getGenreListSoapIn" />
      <wsdl:output message="tns:getGenreListSoapOut" />
    </wsdl:operation>
    <wsdl:operation name="getBandPricePerShow">
      <wsdl:input message="tns:getBandPricePerShowSoapIn" />
      <wsdl:output message="tns:getBandPricePerShowSoapOut" />
    </wsdl:operation>
    <wsdl:operation name="getBandsForADate">
      <wsdl:input message="tns:getBandsForADateSoapIn" />
      <wsdl:output message="tns:getBandsForADateSoapOut" />
    </wsdl:operation>
    <wsdl:operation name="getNextEmptySlot">
      <wsdl:input message="tns:getNextEmptySlotSoapIn" />
      <wsdl:output message="tns:getNextEmptySlotSoapOut" />
    </wsdl:operation>
    <wsdl:operation name="bookABand">
      <wsdl:input message="tns:bookABandSoapIn" />
      <wsdl:output message="tns:bookABandSoapOut" />
    </wsdl:operation>
    <wsdl:operation name="cancelBookingById">
      <wsdl:input message="tns:cancelBookingByIdSoapIn" />
      <wsdl:output message="tns:cancelBookingByIdSoapOut" />
    </wsdl:operation>
    <wsdl:operation name="cancelBooking">
      <wsdl:input message="tns:cancelBookingSoapIn" />
      <wsdl:output message="tns:cancelBookingSoapOut" />
    </wsdl:operation>
    <wsdl:operation name="getBandSchedule">
      <wsdl:input message="tns:getBandScheduleSoapIn" />
      <wsdl:output message="tns:getBandScheduleSoapOut" />
    </wsdl:operation>
  </wsdl:portType>
  <wsdl:binding name="band_wsSoap" type="tns:band_wsSoap">
    <soap:binding transport="http://schemas.xmlsoap.org/soap/http" />
    <wsdl:operation name="getBandName">
      <soap:operation soapAction="http://tempuri.org/getBandName" style="document" />
      <wsdl:input>
        <soap:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getBandDescription">
      <soap:operation soapAction="http://tempuri.org/getBandDescription" style="document" />
      <wsdl:input>
        <soap:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getBandsList">
      <soap:operation soapAction="http://tempuri.org/getBandsList" style="document" />
      <wsdl:input>
        <soap:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getBandsListForGenreAndDate">
      <soap:operation soapAction="http://tempuri.org/getBandsListForGenreAndDate" style="document" />
      <wsdl:input>
        <soap:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getBandsListForGenre">
      <soap:operation soapAction="http://tempuri.org/getBandsListForGenre" style="document" />
      <wsdl:input>
        <soap:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getGenreList">
      <soap:operation soapAction="http://tempuri.org/getGenreList" style="document" />
      <wsdl:input>
        <soap:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getBandPricePerShow">
      <soap:operation soapAction="http://tempuri.org/getBandPricePerShow" style="document" />
      <wsdl:input>
        <soap:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getBandsForADate">
      <soap:operation soapAction="http://tempuri.org/getBandsForADate" style="document" />
      <wsdl:input>
        <soap:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getNextEmptySlot">
      <soap:operation soapAction="http://tempuri.org/getNextEmptySlot" style="document" />
      <wsdl:input>
        <soap:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="bookABand">
      <soap:operation soapAction="http://tempuri.org/bookABand" style="document" />
      <wsdl:input>
        <soap:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="cancelBookingById">
      <soap:operation soapAction="http://tempuri.org/cancelBookingById" style="document" />
      <wsdl:input>
        <soap:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="cancelBooking">
      <soap:operation soapAction="http://tempuri.org/cancelBooking" style="document" />
      <wsdl:input>
        <soap:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getBandSchedule">
      <soap:operation soapAction="http://tempuri.org/getBandSchedule" style="document" />
      <wsdl:input>
        <soap:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
  </wsdl:binding>
  <wsdl:binding name="band_wsSoap12" type="tns:band_wsSoap">
    <soap12:binding transport="http://schemas.xmlsoap.org/soap/http" />
    <wsdl:operation name="getBandName">
      <soap12:operation soapAction="http://tempuri.org/getBandName" style="document" />
      <wsdl:input>
        <soap12:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap12:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getBandDescription">
      <soap12:operation soapAction="http://tempuri.org/getBandDescription" style="document" />
      <wsdl:input>
        <soap12:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap12:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getBandsList">
      <soap12:operation soapAction="http://tempuri.org/getBandsList" style="document" />
      <wsdl:input>
        <soap12:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap12:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getBandsListForGenreAndDate">
      <soap12:operation soapAction="http://tempuri.org/getBandsListForGenreAndDate" style="document" />
      <wsdl:input>
        <soap12:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap12:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getBandsListForGenre">
      <soap12:operation soapAction="http://tempuri.org/getBandsListForGenre" style="document" />
      <wsdl:input>
        <soap12:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap12:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getGenreList">
      <soap12:operation soapAction="http://tempuri.org/getGenreList" style="document" />
      <wsdl:input>
        <soap12:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap12:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getBandPricePerShow">
      <soap12:operation soapAction="http://tempuri.org/getBandPricePerShow" style="document" />
      <wsdl:input>
        <soap12:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap12:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getBandsForADate">
      <soap12:operation soapAction="http://tempuri.org/getBandsForADate" style="document" />
      <wsdl:input>
        <soap12:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap12:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getNextEmptySlot">
      <soap12:operation soapAction="http://tempuri.org/getNextEmptySlot" style="document" />
      <wsdl:input>
        <soap12:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap12:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="bookABand">
      <soap12:operation soapAction="http://tempuri.org/bookABand" style="document" />
      <wsdl:input>
        <soap12:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap12:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="cancelBookingById">
      <soap12:operation soapAction="http://tempuri.org/cancelBookingById" style="document" />
      <wsdl:input>
        <soap12:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap12:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="cancelBooking">
      <soap12:operation soapAction="http://tempuri.org/cancelBooking" style="document" />
      <wsdl:input>
        <soap12:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap12:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
    <wsdl:operation name="getBandSchedule">
      <soap12:operation soapAction="http://tempuri.org/getBandSchedule" style="document" />
      <wsdl:input>
        <soap12:body use="literal" />
      </wsdl:input>
      <wsdl:output>
        <soap12:body use="literal" />
      </wsdl:output>
    </wsdl:operation>
  </wsdl:binding>
  <wsdl:service name="band_ws">
    <wsdl:port name="band_wsSoap" binding="tns:band_wsSoap">
      <soap:address location="http://localhost:1304/band_ws.asmx" />
    </wsdl:port>
    <wsdl:port name="band_wsSoap12" binding="tns:band_wsSoap12">
      <soap12:address location="http://localhost:1304/band_ws.asmx" />
    </wsdl:port>
  </wsdl:service>
</wsdl:definitions>