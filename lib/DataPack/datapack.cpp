#include "datapack.h"

DataPack::DataPack(){
    cursor = 0;
}

bool DataPack::addData(uint8_t index, byte msb, byte lsb){
    if(cursor<87){
        buffer[cursor] = index; cursor++;
        buffer[cursor] = msb; cursor++;
        buffer[cursor] = lsb; cursor++;
        return true;
    }
    return false;
}

bool DataPack::addData(uint8_t index, int16_t data){
    byte data_msb = byte(data >> 8);
    byte data_lsb = byte(data);
    return addData(index, data_msb, data_lsb);
}

void DataPack::clear(){
    for(int i=0; i<90; i++) {
        buffer[i] = 0;
    }
    cursor = 0;
}

int8_t DataPack::calCheckSum(){
    byte sum = 0;
    for(int i=0;i<cursor; i++){
        sum+=buffer[i];
    }
    return byte(0xFF-sum);
}

void DataPack::write(Stream& inp, uint8_t ori, uint8_t dest){
    inp.write(0x7E);
    inp.write(ori);
    inp.write(dest);
    inp.write(cursor);
    for(int i=0;i<cursor;i++){
        inp.write(buffer[i]);
    }
    inp.write(calCheckSum());
    clear();
}

bool DataPack::available(Stream& inp){
    if(inp.available()){
        uint8_t header = inp.read();
        if(header == 0x7E){
            uint8_t head[3];
            inp.readBytes(head, 3);
            uint8_t temp_origin = head[0];
            uint8_t temp_destination = head[1];
            uint8_t len = head[2];
            uint8_t temp_buffer[len+1];
            inp.readBytes(temp_buffer, len+1);
            uint8_t rec_checksum = temp_buffer[len];
            uint16_t sum = 0;
            for(int i=0;i<len; i++){
                sum += temp_buffer[i];
            }
            sum = 0xFF - byte(sum);
            if(rec_checksum == sum){
                origin = temp_origin;
                destination = temp_destination;
                for(int i=0; i<len; i++){
                    inp_buffer[i] = temp_buffer[i];
                }
                inp_data = len/3;
                return true;
            }
        }
    }
    return false;
}

uint8_t* DataPack::getKeys(){
  uint8_t* out = (uint8_t *) calloc(inp_data, sizeof(uint8_t));
  for(int i=0;i<inp_data;i++){
    out[i] = inp_buffer[(3*i)];
  }
  return out;
}

uint8_t DataPack::inWaiting(){
  return inp_data;
}

bool DataPack::hasKey(uint8_t key){
  for(int i = 0; i < inp_data*3 ; i+=3){
    if(key == inp_buffer[i]){
      return true;
    }
  }
  return false;
}

uint16_t DataPack::getData(uint8_t key){
  for(int i = 0; i < inp_data*3 ; i+=3){
    if(key == inp_buffer[i]){
      return (inp_buffer[i+1] << 8) | (inp_buffer[i+2]);
    }
  }
  return 0;
}

bool DataPack::checkOrigin(uint8_t ori){
    return (origin == ori);
}

bool DataPack::checkDestination(uint8_t des){
    return (destination == des);
}