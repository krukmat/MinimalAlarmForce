/**
 * @description       : 
 * @author            : Matias Kruk
 * @group             : 
 * @last modified on  : 05-20-2021
 * @last modified by  : Matias Kruk
 * Modifications Log 
 * Ver   Date         Author        Modification
 * 1.0   05-20-2021   Matias Kruk   Initial Version
**/
import { LightningElement, api, wire } from 'lwc';
import armAlarm from '@salesforce/apex/AlarmForceCallout.armAlarm';
import { getRecord } from 'lightning/uiRecordApi';

const FIELDS = [
    'Sensor__c.channel__c'
];

export default class ArmAlarm extends LightningElement {
    @api recordId;
    @api value;
    @wire(getRecord, { recordId: '$recordId', fields: FIELDS })
    sensor;

    channel() {
        return this.sensor.data.fields.channel__c.value;
    }

    handleCheckboxChange() {
        this.value =  this.template.querySelectorAll('lightning-input')[0].checked;
    }

    handleClick(){
       let channelId = this.channel();
        armAlarm({channel_id: channelId, value: this.value} ).then(response => {
            // TODO: MEnsaje todo OK
            console.info('Success!!');
        }).catch(error => {
            console.log('Error: ' +error.body.message);
        });
    }
}