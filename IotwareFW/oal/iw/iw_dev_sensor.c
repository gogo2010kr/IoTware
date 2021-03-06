/*
Copyright (C) <2020> <ETRI>

This License and Service Agreement (LSA) applies to all works and their derivative works based on source form version and object form version of IoTware Project. Currently, the LSA for IoTware Project has two policies, 'Open Source License' and 'Commercial License'. Therefore, all works including the source code and executable code of IoTware Project and derivative works based thereon are subject to either 'Open Source License' or 'Commercial License' depending on the user's needs and purpose. Details related to the selection of the applicable license are specified in this LSA. If you use any or all of IoTware Project in any form, you are deemed to have consented to this LSA. If you breach any of the terms and conditions set forth in this LSA, you are solely responsible for any losses or damages incurred by Electronics and Communications Research Institute (ETRI), and ETRI assume no responsibility for you or any third party.
If you use the source form version or object form version of IoTware Project in whole or in part to develop a code or a derivative work, and you want to commercialize the result in some form, you will be covered under a commercial license. And if you are subject to a commercial license, the contract for the use of IoTware Project is subject to TECHNOLOGY LICENSE AGREEMENT of ETRI. You acknowledge that ETRI has all legal rights, title and interest, including intellectual property rights in the IoTware Project (regardless of whether such intellectual property rights are registered or where such rights exist) and agree with no objection thereto. Except as provided in a subsidiary agreement, nothing in this LSA grants you the right to use IoTware Project or the name, service mark, logo, domain name and other unique identification marks of ETRI.
If you use the source form version or object form version of IoTware Project in whole or in part to develop a code or a derivative work, and you do not commercialize the result in any form, you will be covered under an open source license. IoTware Project is in accordance with Free Software Foundation (FSF)'s open source policy, and is allowed to use it in the appropriate scope and manner, and you must comply with the applicable open source license policy applied to IoTware Project. IoTware Project is, in principle, subject to GNU Lesser General Public License version 2.1 (LGPLv2.1). If you have acquired all or a part of the IoTware Project in any way and it is subject to a license other than the open source license described above, please contact the following address for the technical support and other inquiries before use, and check the usage information.
*/

#include "iw_common.h"
#include "iw_dev_sensor.h"

extern unsigned int g_uart_using_info[IF_ID_MAX]; 
extern unsigned char g_i2c_using_info[IF_ID_MAX]; 
static sensor_info_t si; /* sensor info */

void iw_register_sensor(sensor_type_t type, if_type_t iface, if_id_t iface_id, unsigned int info, char *name)
{
	unsigned char dev_id = si.dev_num;
	
	sensor_dev_info_t *dev = &si.dev[dev_id];
	
	if(iface == IF_TYPE_UART){
		if(g_uart_using_info[iface_id] == 0){
			g_uart_using_info[iface_id] = info;
		}else{
			iw_printf("Check uart\n");
			return;
		}
		//printf("(%d)uart%d %s\n", type, iface_id, name);
	}
	else if(iface == IF_TYPE_I2C){
		if(g_i2c_using_info[iface_id] == 0){
			g_i2c_using_info[iface_id] = 1;
		}else{
			iw_printf("Check (%d i2c%d %s)\n",type, iface_id, name);
			return;
		}
	}

	dev->type = type; /* sensor type */
	dev->iface = iface;
	dev->iface_id = iface_id;
	memcpy(dev->name, name, MAX_DRIVER_NAME_LEN); 
	si.dev[dev_id].dev_id = dev_id+1;
	si.dev_num++;

	//iw_printf("iw_register_sensor(%d, %d, %d, %d, %s)\n", type, iface, iface_id, info, name);
}

void iw_register_sensor_driver(sensor_driver_t *sensor_driver)
{
	// position of driver to be registered
	sensor_driver_t *driver = &si.driver[si.driver_num];

	// copy the driver's information(name, function)
	memcpy(driver->name ,sensor_driver->name, MAX_DRIVER_NAME_LEN);
	driver->func = sensor_driver->func;
	
	// add count of drivers
	si.driver_num++;
}

iw_error_t iw_register_sensor_(sensor_type_t dev_type, const char *name, if_type_t if_type, uint32_t if_num, void *if_arg, sensor_driver_t *dev_driver)
{
	if(MAX_SENSOR_DEV <= si.dev_num) return IW_FAIL;

	uint8_t dev_id = si.dev_num;
	sensor_dev_info_t *dev = &si.dev[dev_id];

	dev->type = dev_type;
	strncpy(dev->name, name, MAX_DRIVER_NAME_LEN);
	dev->iface = if_type;
	dev->iface_id = (if_id_t)if_num;
	//si.dev[dev_id].dev_id = dev_id+1;
	dev->func = dev_driver->func;
	si.dev_num++;

	//iw_printf("iw_register_sensor(%d, %s, %d, %d)\n", dev->type, dev->name, dev->iface, dev->iface_id);

	return IW_SUCCESS;
}


uint8_t iw_get_registered_sensor_num(void)
{
	return si.dev_num;
}

static inline void _iw_get_sensor_info(uint8_t id, 
                                       registered_sensor_info_t *info)
{
	// copy the sensor's information(id, type, name)
	info[id].dev_id = id;
	info[id].type = si.dev[id].type;
	strcpy((char *) info[id].name, si.dev[id].name);
}

iw_error_t iw_get_all_sensors_info(registered_sensor_info_t *info)
{
	uint8_t id; // contains a index of each devices

	// copy all sensor's information
	for (id = 0; id < si.dev_num; id++)
		_iw_get_sensor_info(id, info);

	return IW_SUCCESS;
}

iw_error_t iw_get_sensor_info(sensor_type_t type, char *name,
                              registered_sensor_info_t *info)
{
	uint8_t id; // contains a index of each devices
	
	// check parameters is null
	if (!name || !info)
		return IW_FAIL;

	for (id = 0; id < si.dev_num; id++) {
		
		// check device type and name
		if (si.dev[id].type == type 
			&& strcmp(si.dev[id].name, name) == 0) {
			
			// copy the sensor's information
			_iw_get_sensor_info(id, info);
			
			return IW_SUCCESS;
		}
	}

	return IW_FAIL;
}

sensor_dev_info_t *iw_get_sensor_dev_info(uint8_t dev_id)
{
	if(si.dev_num <= dev_id) return 0;
	if(si.dev[dev_id].type == 0) return 0;

	return &si.dev[dev_id];
}

iw_error_t iw_get_sensor_devid(sensor_type_t type, char *name, unsigned char *devid)
{
	sensor_dev_info_t *dev;
	int i = 0, found = 0;

	for(i=0; i<si.dev_num; i++)	
	{
		dev = &si.dev[i];

		if(type>0 && (dev->type & type)){
			if(name != NULL){
				if(!strcmp(name, dev->name)){
					*devid = i;
					found = 1;
					break;
				}				
			}else{				
				*devid = i;
				found = 1;
			}
		}		
	}

	if(found)
		return IW_SUCCESS;	

	return IW_FAIL;
}
iw_error_t iw_get_sensor_dev_id_by_type(sensor_type_t type, uint8_t *dev_id)
{
	int32_t i;
	for(i = 0; i < si.dev_num; i++) {
		sensor_dev_info_t *dev = &si.dev[i];

		if((dev->type & type) == type) {
			*dev_id = i;
			return IW_SUCCESS;
		}
	}

	return IW_FAIL;
}

iw_error_t iw_get_sensor_dev_id_by_name(char *name, uint8_t *dev_id)
{
	int32_t i; 
	for(i = 0; i < si.dev_num; i++) {
		sensor_dev_info_t *dev = &si.dev[i];

		if(dev->type != 0 && strcmp(name, dev->name) == 0) {
			*dev_id = i;
			return IW_SUCCESS;
		}		
	}

	return IW_FAIL;
}

iw_error_t iw_init_sensor(unsigned char dev_id)
{
	//unsigned char driver_num = si.driver_num;
	//unsigned char i;
	sensor_dev_info_t *dev = &si.dev[dev_id];
	//bool driver_found = false;

	//iw_printf("iw_init_sensor(%d)\n", dev_id);

	if(si.dev_num <= dev_id) return IW_FAIL;
	if(dev->type == 0) return IW_FAIL;
	if(dev->func == 0) return IW_FAIL;
	if(dev->func->init == 0) return IW_FAIL;
#if 0
	for(i = 0; i < driver_num; i++) {
		if(memcmp(dev->name, si.driver[i].name, strlen((char *)dev->name)) == 0) {
			dev->func = si.driver[i].func;
			driver_found = true;
			break;
		}
	}

	if(driver_found == false){
		dev->func = NULL;
		iw_printf("iw_init_sensor(%d) err \n", dev_id);
		return IW_FAIL;
	}
#endif

	return dev->func->init(dev);
	//return 0;
}

iw_error_t iw_deinit_sensor(unsigned char dev_id)		
{
	if(si.dev[dev_id].func && si.dev[dev_id].func->deinit)
		return si.dev[dev_id].func->deinit(&si.dev[dev_id]);
	
	return IW_FAIL;
}

iw_error_t iw_is_ready_sensor(unsigned char dev_id, dev_init_status_t *status) 		
{
	if(si.dev[dev_id].func && si.dev[dev_id].func->is_ready)
		return si.dev[dev_id].func->is_ready(&si.dev[dev_id], status);
	
	return IW_FAIL;
}

iw_error_t iw_read_sensor(unsigned char dev_id, sensor_type_t type, void *buff, unsigned int len)
{
	if(si.dev[dev_id].func && si.dev[dev_id].func->read) {
		return si.dev[dev_id].func->read(&si.dev[dev_id], type, buff, len);
	}
	else {
		iw_printf("iw_read_sensor(%d, %d, %d, %d) err\n", dev_id, type, buff, len);
	}
	
	return IW_FAIL; 
}

iw_error_t iw_get_capa_sensor(unsigned char dev_id, wake_src_type_t *wake_src)
{
	if(si.dev[dev_id].func && si.dev[dev_id].func->get_capa)
		return si.dev[dev_id].func->get_capa(&si.dev[dev_id], wake_src);
	
	return IW_FAIL; 
}

iw_error_t iw_set_config_sensor(unsigned char dev_id, wake_src_type_t wake_src, void *config)
{
	if(si.dev[dev_id].func && si.dev[dev_id].func->set_config)
		return si.dev[dev_id].func->set_config(&si.dev[dev_id], wake_src, config);
	
	return IW_FAIL; 
}

iw_error_t iw_get_config_sensor(unsigned char dev_id, wake_src_type_t *wake_src, void *config)
{
	if(si.dev[dev_id].func && si.dev[dev_id].func->get_config)
		return si.dev[dev_id].func->get_config(&si.dev[dev_id], wake_src, config);
	
	return IW_FAIL; 
}

iw_error_t iw_set_pwrmode_sensor(unsigned char dev_id, sensor_pwr_mode_t pwr_mode)
{
	if(si.dev[dev_id].func && si.dev[dev_id].func->set_power)
		return si.dev[dev_id].func->set_power(&si.dev[dev_id], pwr_mode);
	
	return IW_FAIL; 
}

iw_error_t iw_get_pwrmode_sensor(unsigned char dev_id, sensor_pwr_mode_t *pwr_mode) 
{
	if(si.dev[dev_id].func && si.dev[dev_id].func->get_power)
		return si.dev[dev_id].func->get_power(&si.dev[dev_id], pwr_mode);
	
	return IW_FAIL;
}

