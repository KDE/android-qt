package com.nokia.qt.android;

import android.util.Log;

public class AllContacts
{
	public MyContacts[] m_allAndroidContacts;
	
	public AllContacts(int count)
	{
		m_allAndroidContacts = new MyContacts[count];
	}
	
	public void buildContacts(int i,String id,String name,NameData names,PhoneNumber[] phoneNumberArray,
			                  EmailData[] emailArray,String note,AddressData[] addressArray,
			                  OrganizationalData[] orgArray,String birthday,String anniversary,
			                  String nickName,String[] urlArray)
	{
	   	    m_allAndroidContacts[i] = new MyContacts(id,name,names,phoneNumberArray,emailArray,note,addressArray,orgArray,birthday,anniversary,nickName,urlArray);
	}
	
}


class MyContacts
{
	String   m_dispalyName;
	NameData m_names;
	PhoneNumber[] m_phoneNumbers;
	EmailData[] m_email;

	String   m_contactNote;
	AddressData[] m_address;

	OrganizationalData[] m_organizations;

	String   m_contactID;
	String   m_contactBirthDay;
	String   m_contactAnniversary;
	String   m_contactNickName;
	String[] m_contactUrls;


 public MyContacts(String iid,String name,NameData names,PhoneNumber[] phonenumberArray,
			EmailData[] emailArray,String note,AddressData[] addressArray,
			OrganizationalData[] organizationArray,String birthday,
			String anniversary,String nickName,String[] urlArray)

	{
		m_contactID = new String(iid);
		m_dispalyName = new String(name);
		
		if(names != null)
		{
		m_names = new NameData(names.m_firstName,names.m_lastName,names.m_middleName,names.m_prefix,names.m_suffix);
		}
		else
		{
		m_names = new NameData(new String(),new String(),new String(),new String(),new String());
		}
		int i=0;
		int numbercount = phonenumberArray.length;
		if(numbercount>0)
		{

			m_phoneNumbers = new PhoneNumber[numbercount];
			for(i=0;i<numbercount;i++)
			{
				m_phoneNumbers[i] = phonenumberArray[i];
			}
		}
		else
		{

			m_phoneNumbers = new PhoneNumber[1];
			m_phoneNumbers[0] = new PhoneNumber(new String(), 0);
		}

		int emailcount = emailArray.length;
		if(emailcount>0)
		{
			m_email = new EmailData[emailcount];
			for(i=0;i<emailcount;i++)
			{
				m_email[i] = emailArray[i];
			}
		}
		else
		{
			m_email = new EmailData[1];
			m_email[0] = new EmailData(new String(),0);
		}
		
		if(note != null)
		{
			m_contactNote = new String(note);
		}
		else
		{
			m_contactNote = new String();
		}
		int addrcount = addressArray.length; 
		if(addrcount>0)
		{
			m_address = new AddressData[addrcount];
			for(i=0;i<addrcount;i++)
			{
				m_address[i] = addressArray[i];	
			}
		}
		else
		{
			m_address = new AddressData[1];
			m_address[0] = new AddressData(new String(),new String(), new String(), new String(), new String(), new String(), 0);
		}
		
		int orgcount = organizationArray.length;
		if(orgcount>0)
		{
			m_organizations = new OrganizationalData[orgcount];
			for(i=0;i<orgcount;i++)
			{
				m_organizations[i] = organizationArray[i];
			}
		}
		else
		{
			m_organizations = new OrganizationalData[1];
			m_organizations[0] = new OrganizationalData(new String(),new String(),0);
		}

		if(birthday != null)
		{
			m_contactBirthDay = new String(birthday);
		}
		else
		{
			m_contactBirthDay =new String();
		}
		
		if(anniversary != null)
		{
			m_contactAnniversary = new String(anniversary);
		}
		else
		{
			m_contactAnniversary =new String();
		}
		
		if(nickName != null)
		{
			m_contactNickName = new String(nickName);
		}
		else
		{
			m_contactNickName = new String();
		}
		
		int urlcount = urlArray.length;
		if(urlcount>0)
		{
			m_contactUrls = new String[urlcount];
			for(i=0;i<urlcount;i++)
			{
				m_contactUrls[i] = urlArray[i];
			}
		}
		else
		{
			m_contactUrls = new String[1];
			m_contactUrls[0]= new String();
		}
		
	}

} 
class PhoneNumber 
{
	String m_number;
	int m_type;
	public PhoneNumber(String number,int type)
	{
		m_number = new String(number);
		m_type = type;
	}
}
class EmailData
{
	String m_email;
	int m_type;
	public EmailData(String email,int type)
	{
		m_email = new String(email);
		m_type = type;
	}
	
}
class OrganizationalData
{
	String m_organization;
	String m_title;
	int m_type;
	public OrganizationalData(String organization,String title,int type)
	{
		m_organization = new String(organization);
		m_title = new String(title);
		m_type = type;
	}
}
class AddressData
{
	String m_pobox;
	String m_street;
	String m_city;
	String m_region;
	String m_postCode;
	String m_country;
	int m_type;
	public AddressData(String pobox,String street,String city,String region,String postCode,String country,int type)
	{
		m_pobox = new String(pobox);
		m_street = new String(street);
		m_city = new String(city);
		m_region = new String(region);
		m_postCode = new String(postCode);
		m_country = new String(country);
		m_type = type;
		
	}
	
}
class NameData
{
	String m_firstName;
	String m_lastName;
	String m_middleName;
	String m_prefix;
	String m_suffix;
	public NameData(String firstName,String lastName,String middleName,String prefix,String suffix)
	{
		m_firstName = new String(firstName);
		m_lastName = new String(lastName);
		m_middleName = new String(middleName);
		m_prefix = new String(prefix);
		m_suffix = new String(suffix);
	}
}


