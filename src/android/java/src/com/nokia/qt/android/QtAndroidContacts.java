package com.nokia.qt.android;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import android.R.bool;
import android.net.Uri;
import android.os.Handler;
import android.provider.ContactsContract.CommonDataKinds.Email;
import android.provider.ContactsContract.CommonDataKinds.Event;
import android.provider.ContactsContract.CommonDataKinds.Nickname;
import android.provider.ContactsContract.CommonDataKinds.Note;
import android.provider.ContactsContract.CommonDataKinds.Organization;
import android.provider.ContactsContract.CommonDataKinds.Phone;
import android.provider.ContactsContract.CommonDataKinds.StructuredName;
import android.provider.ContactsContract.CommonDataKinds.StructuredPostal;
import android.provider.ContactsContract.CommonDataKinds.Website;
import android.provider.ContactsContract.Contacts.Data;
import android.provider.ContactsContract.RawContacts;
import android.util.Log;
import android.database.*;
import android.accounts.Account;
import android.accounts.OnAccountsUpdateListener;
import android.content.ContentResolver;
import android.content.ContentUris;
import android.provider.ContactsContract;
import android.content.ContentValues; 

public class QtAndroidContacts
{
	private static QtActivity _m_instance;
	private String TAG = "QTMOBILITYCONTACTS";
	private static AllContacts m_androidContacts;
	public static void setContactsActivity(QtActivity mActivity)
	{
		_m_instance = mActivity;
	}
	@SuppressWarnings("unused")
	private void getQtContacts()
	{
		Cursor c=_m_instance.getContentResolver().query(ContactsContract.Contacts.CONTENT_URI,null, null, null, null);	
		m_androidContacts = new AllContacts(c.getCount());
		for(int i=0;i<c.getCount();i++)
		{
			c.moveToNext();
			String id = c.getString(c.getColumnIndex(ContactsContract.Contacts._ID));
			String displayName = c.getString(c.getColumnIndex(ContactsContract.Contacts.DISPLAY_NAME));
			NameData nameData = getNameDetails(id);
			PhoneNumber[] numberArray = {};
			if (Integer.parseInt(c.getString(c.getColumnIndex(ContactsContract.Contacts.HAS_PHONE_NUMBER))) > 0) 
			{
				numberArray = getPhoneNumberDetails(id);
			}
			EmailData[] emailArray = getEmailAddressDetails(id);
			String note = getNoteDetails(id);
			AddressData[] addressArray= getAddressDetails(id);
			OrganizationalData[] organizationArray = getOrganizationDetails(id);
			String birthday = getBirthdayDetails(id);
			String[] urlArray =  getUrlDetails(id);
			String anniversary = getAnniversaryDetails(id);
			String nickNameg = getNickNameDetails(id);
			m_androidContacts.buildContacts(i,id,displayName,nameData,numberArray,emailArray,note,addressArray,organizationArray,birthday,anniversary,nickNameg,urlArray);
		}
	}
    private NameData getNameDetails(String id)
    {
    	String nameWhere = ContactsContract.Data.CONTACT_ID + " = ? AND " + ContactsContract.Data.MIMETYPE + " = ?"; 
		String[] nameWhereParams = new String[]{id, 
				ContactsContract.CommonDataKinds.StructuredName.CONTENT_ITEM_TYPE}; 
		Cursor nameCur = _m_instance.getContentResolver().query(ContactsContract.Data.CONTENT_URI, 
				null, nameWhere, nameWhereParams, null);
		if(nameCur != null && nameCur.moveToFirst())
		{
			
			String firstName = nameCur.getString(nameCur.getColumnIndex(ContactsContract.CommonDataKinds.StructuredName.GIVEN_NAME));
			if(firstName == null)
				firstName = new String();
			String lastName = nameCur.getString(nameCur.getColumnIndex(ContactsContract.CommonDataKinds.StructuredName.FAMILY_NAME));
			if(lastName == null)
				lastName = new String();
			String middleName = nameCur.getString(nameCur.getColumnIndex(ContactsContract.CommonDataKinds.StructuredName.MIDDLE_NAME));
			if(middleName == null)
				middleName = new String();
			String prefix  = nameCur.getString(nameCur.getColumnIndex(ContactsContract.CommonDataKinds.StructuredName.PREFIX));
			if(prefix == null)
				prefix = new String();
			String suffix = nameCur.getString(nameCur.getColumnIndex(ContactsContract.CommonDataKinds.StructuredName.SUFFIX));
			if(suffix == null)
				suffix = new String();
			NameData nameDetails = new NameData(firstName,lastName,middleName,prefix,suffix);

			nameCur.close();

			return nameDetails;
			
		}
		return null;
	}

	private PhoneNumber[] getPhoneNumberDetails(String id)
	{
		Cursor pCur = _m_instance.getContentResolver().query(
				ContactsContract.CommonDataKinds.Phone.CONTENT_URI, 
				null, 
				ContactsContract.CommonDataKinds.Phone.CONTACT_ID +" = ?", 
				new String[]{id}, null);
		if(pCur!= null)
		{
			int i=0;
			PhoneNumber[] numbers = new PhoneNumber[pCur.getCount()];
			while (pCur.moveToNext())
			{
				String number= pCur.getString(pCur.getColumnIndex(ContactsContract.CommonDataKinds.Phone.NUMBER));
				int type =Integer.parseInt(pCur.getString(pCur.getColumnIndex(ContactsContract.CommonDataKinds.Phone.TYPE)));
				numbers[i] = new PhoneNumber(number, type);
				i++;
			} 
			pCur.close();
			return numbers;
		}
		return null;
	}

	private EmailData[] getEmailAddressDetails(String id)
	{
		Cursor emailCur = _m_instance.getContentResolver().query( 
				ContactsContract.CommonDataKinds.Email.CONTENT_URI, 
				null,
				ContactsContract.CommonDataKinds.Email.CONTACT_ID + " = ?", 
				new String[]{id}, null);
		if(emailCur!= null)
		{
			EmailData[] emails = new EmailData[emailCur.getCount()];
			int i=0;
			while (emailCur.moveToNext())
			{ 
				String email = emailCur.getString(emailCur.getColumnIndex(ContactsContract.CommonDataKinds.Email.DATA));
				String type = emailCur.getString(emailCur.getColumnIndex(ContactsContract.CommonDataKinds.Email.TYPE));
				emails[i] = new EmailData(email, Integer.parseInt(type));
				i++;
			}
			emailCur.close();
		   return emails;
		}
		return null;
	}
	private String getNoteDetails(String id)
	{
		String noteWhere = ContactsContract.Data.CONTACT_ID + " = ? AND " + ContactsContract.Data.MIMETYPE + " = ?"; 
		String[] noteWhereParams = new String[]{id, ContactsContract.CommonDataKinds.Note.CONTENT_ITEM_TYPE}; 
		Cursor noteCur = _m_instance.getContentResolver().query(ContactsContract.Data.CONTENT_URI, null, noteWhere, noteWhereParams, null);
		String note = null;
		if (noteCur.moveToFirst()) { 
			note = noteCur.getString(noteCur.getColumnIndex(ContactsContract.CommonDataKinds.Note.NOTE));
		} 
		noteCur.close();
		return note;
	}
	private String getBirthdayDetails(String id)
	{
		Cursor birthDayCur =_m_instance.getContentResolver().query( ContactsContract.Data.CONTENT_URI, new String[] { Event.DATA }, ContactsContract.Data.CONTACT_ID + "=" + id + " AND " + Data.MIMETYPE + "= '" + Event.CONTENT_ITEM_TYPE + "' AND " + Event.TYPE + "=" + Event.TYPE_BIRTHDAY, null, null);
		if(birthDayCur.moveToFirst())
		{
			DateFormat df = DateFormat.getDateInstance(DateFormat.LONG);
			String birthday = null;
			try {
				Date mydate = df.parse( birthDayCur.getString(0));
				SimpleDateFormat newformat = new SimpleDateFormat("dd:MM:yyyy");
				birthday = newformat.format(mydate);
			} catch (ParseException e) {

				e.printStackTrace();
			}
			birthDayCur.close();
			return birthday;
		}
		return null;
	}
	private String getAnniversaryDetails(String id)
	{
		Cursor anniversaryCur =_m_instance.getContentResolver().query( ContactsContract.Data.CONTENT_URI, new String[] { Event.DATA }, ContactsContract.Data.CONTACT_ID + "=" + id + " AND " + Data.MIMETYPE + "= '" + Event.CONTENT_ITEM_TYPE + "' AND " + Event.TYPE + "=" + Event.TYPE_ANNIVERSARY, null, ContactsContract.Data.DISPLAY_NAME);
		if(anniversaryCur.moveToFirst())
		{
			DateFormat df = DateFormat.getDateInstance(DateFormat.LONG);
			String anniversary = null;  
			try {
				Date mydate = df.parse( anniversaryCur.getString(0));
				SimpleDateFormat newformat = new SimpleDateFormat("dd:MM:yyyy");
				anniversary = newformat.format(mydate);
			} catch (ParseException e) {

				e.printStackTrace();
			}
			anniversaryCur.close();
			return anniversary;
		}
		return null;
	}
	private String getNickNameDetails(String id)
	{
		String nickWhere = ContactsContract.Data.CONTACT_ID + " = ? AND " + ContactsContract.Data.MIMETYPE + " = ?"; 
		String[] nickWhereParams = new String[]{id, 
				ContactsContract.CommonDataKinds.Nickname.CONTENT_ITEM_TYPE}; 
		Cursor nickCur = _m_instance.getContentResolver().query(ContactsContract.Data.CONTENT_URI, 
				null, nickWhere, nickWhereParams, null);
		if (nickCur.moveToFirst()) { 
			String nickName= nickCur.getString(nickCur.getColumnIndex(ContactsContract.CommonDataKinds.Nickname.DATA));
			nickCur.close();
			return nickName;
		} 
		return null;
	}
	private OrganizationalData[] getOrganizationDetails(String id)
	{ 
		String orgWhere = ContactsContract.Data.CONTACT_ID + " = ? AND " + ContactsContract.Data.MIMETYPE + " = ?"; 
		String[] orgWhereParams = new String[]{id, 
				ContactsContract.CommonDataKinds.Organization.CONTENT_ITEM_TYPE}; 
		Cursor orgCur = _m_instance.getContentResolver().query(ContactsContract.Data.CONTENT_URI, 
				null, orgWhere, orgWhereParams, null);
		if(orgCur!= null)
		{
			OrganizationalData[] organizations = new OrganizationalData[orgCur.getCount()];
			int i=0;
			while (orgCur.moveToNext())
			{ 
				String organization = orgCur.getString(orgCur.getColumnIndex(ContactsContract.CommonDataKinds.Organization.DATA));
				if(organization == null)
					organization = new String();
				String title = orgCur.getString(orgCur.getColumnIndex(ContactsContract.CommonDataKinds.Organization.TITLE));
				if(title == null)
					title = new String();
				String type = orgCur.getString(orgCur.getColumnIndex(ContactsContract.CommonDataKinds.Organization.TYPE));
				organizations[i]= new OrganizationalData(organization, title, Integer.parseInt(type));
				i++;
			}
			orgCur.close();
			return organizations;
		}
		return null;
	}
	private AddressData[] getAddressDetails(String id)
	{
		String addrWhere = ContactsContract.Data.CONTACT_ID + " = ? AND " + ContactsContract.Data.MIMETYPE + " = ?"; 
		String[] addrWhereParams = new String[]{id, 
				ContactsContract.CommonDataKinds.StructuredPostal.CONTENT_ITEM_TYPE}; 
		Cursor addrCur = _m_instance.getContentResolver().query(ContactsContract.Data.CONTENT_URI, 
				null, addrWhere,addrWhereParams, null);
		if(addrCur != null)
		{
			AddressData[] addresses = new AddressData[addrCur.getCount()];
			int i=0;
			while(addrCur.moveToNext()) {
			String pobox = addrCur.getString(addrCur.getColumnIndex(ContactsContract.CommonDataKinds.StructuredPostal.POBOX));
				if(pobox == null)
					pobox = new String();
			String street =addrCur.getString(addrCur.getColumnIndex(ContactsContract.CommonDataKinds.StructuredPostal.STREET));
				if(street == null)
					street = new String();
			String city =addrCur.getString(addrCur.getColumnIndex(ContactsContract.CommonDataKinds.StructuredPostal.CITY));
				if(city == null)
					city = new String();
			String region = addrCur.getString(addrCur.getColumnIndex(ContactsContract.CommonDataKinds.StructuredPostal.REGION));
				if(region == null)
					region = new String();
			String postCode = addrCur.getString(addrCur.getColumnIndex(ContactsContract.CommonDataKinds.StructuredPostal.POSTCODE));
				if(postCode == null)
					postCode = new String();
			String country = addrCur.getString(addrCur.getColumnIndex(ContactsContract.CommonDataKinds.StructuredPostal.COUNTRY));
				if(country == null)
					country = new String();
			String type = addrCur.getString(addrCur.getColumnIndex(ContactsContract.CommonDataKinds.StructuredPostal.TYPE));
				addresses[i] = new AddressData(pobox, street, city, region, postCode, country, Integer.parseInt(type));
				i++;
			} 
			addrCur.close();
			return addresses;
		}
		return null;
	}
	private String[] getUrlDetails(String id)
	{
		String urlWhere = ContactsContract.Data.CONTACT_ID + " = ? AND " + ContactsContract.Data.MIMETYPE + " = ?"; 
		String[] urlWhereParams = new String[]{id, ContactsContract.CommonDataKinds.Website.CONTENT_ITEM_TYPE}; 
		Cursor urlCur = _m_instance.getContentResolver().query(ContactsContract.Data.CONTENT_URI, null, urlWhere, urlWhereParams, null);
		if(urlCur!= null)
		{
			String[] urls = new String[urlCur.getCount()];
			int i=0;
			while (urlCur.moveToNext())
			{ 
				urls[i] =urlCur.getString(urlCur.getColumnIndex(ContactsContract.CommonDataKinds.Website.DATA));
				i++;
			}
			urlCur.close();
			return urls;
		}
		return null;
	}
}
