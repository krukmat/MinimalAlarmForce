/**
 * @description       : 
 * @author            : Matias Kruk
 * @group             : 
 * @last modified on  : 05-22-2021
 * @last modified by  : Matias Kruk
 * Modifications Log 
 * Ver   Date         Author        Modification
 * 1.0   05-20-2021   Matias Kruk   Initial Version
**/
public with sharing class AlarmForceCallout {
    @AuraEnabled
    public static String armAlarm(String channel_id, Boolean value) { 
        try {
            Http http = new Http();
            HttpRequest request = new HttpRequest();
            
            // Provide the Named Credentials
            String parameter = '0';
            if (value){
                parameter = '1';
            }
            request.setEndpoint('http://alarmforce.krukforce.com/alarm/armed?channel_id='+channel_id+'&value='+parameter);
            request.setMethod('GET');        
            System.debug(request.getEndpoint());
            HttpResponse response = http.send(request);
            if (response.getStatusCode() == 200) {
                return response.getBody();
            }
        } catch (Exception e) {
            throw new AuraHandledException(e.getMessage());
        }
        return null;
    }
}
