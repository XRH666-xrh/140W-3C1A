/* vim: set ai et ts=4 sw=4: */
#include "main.h"
#include "pm.h"
#include "uart.h"
#include "debug.h"

/* Private macro -------------------------------------------------------------*/
//#define SPLOG
#ifdef SPLOG
#define LOGSP LOG
#define LOGSPHEX LOG_HEX
#else
#define LOGSP(...)
#define LOGSPHEX(...)
#endif
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const static char head[] = {0xaa, 0x55, 0x00};
i2c_reg_t device;
/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @retval int
  */
void sp86xx_init(uart_recive_stat_t *stat)
{
	if (stat->hinit) stat->hinit();
}

//CRC16_CCITTÐ£Ñé
unsigned short Rx_CRC_CCITT(unsigned char *puchMsg, unsigned int usDataLen)
{
	unsigned char i = 0;
	unsigned short wCRCin = 0x0000;
	unsigned short wCPoly = 0x1021;
	unsigned char wChar = 0;
	while (usDataLen--)
	{
		wChar = *(puchMsg++);
		wCRCin ^= (wChar << 8);
		for(i = 0; i < 8; i++)
		{
			if (wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}return (wCRCin);
}

static void reset_receive(uart_recive_stat_t *stat)
{
	stat->receive_stat = RECEIVE_IDLE;
	stat->receive_len = 0;
	stat->to_receive_len = 0;
}

void sp86xx_recive_process(uart_recive_stat_t *stat)
{
	if (stat->rb->read == stat->rb->write)
	{
		if (stat->receive_stat && HAL_GetTick() - stat->last_receive_time >= BYTE_RECEIVE_TIMEOUT_MS)
		{
			// timeout, abort receive
			reset_receive(stat);
		}
		return;
	}
	stat->last_receive_time = HAL_GetTick();
	while (stat->rb->read != stat->rb->write)
	{
		uint8_t dat = stat->rb->buff[stat->rb->read];
		stat->rb->read = (stat->rb->read+1) % RB_BUFF_SIZE;
		switch(stat->receive_stat)
		{
			case RECEIVE_IDLE:
				if (dat == head[0])
				{
					stat->receive_stat = RECEIVE_HEAD;
					stat->receive_len = 1;
					LOGSP("head\n");
				}
				else
				{
					LOGSP("whead %02x\n", dat);
				}
				break;
			case RECEIVE_HEAD:
			{
				if (dat == head[stat->receive_len++])
				{
					if (stat->receive_len == 3)
					{
						stat->receive_stat = RECEIVE_CMD;
						stat->receive_len = 0;
					}
				}
				else
				{
					LOGSP("whead %d %d\n", dat, stat->receive_len);
					if (dat == head[0])
					{
						stat->receive_len = 1;
					}
					else
					{
						reset_receive(stat);
					}
				}
				break;
			}
			case RECEIVE_CMD:
			{
				if (dat != 0x08)
				{
					reset_receive(stat);
					LOGSP("WRONG CMD %d\n", dat);
				}
				LOGSP("cmd %d\n", dat);
				stat->cmd = dat;
				stat->receive_stat = RECEIVE_LEN;
				break;
			}
			case RECEIVE_LEN:
			{
				if (stat->receive_len == 0)
				{
					if (dat == 0) stat->receive_len = 1;
					else
					{
						LOGSP("WRONG LEN %d\n", dat);
						reset_receive(stat);
					}
				}
				else if (stat->receive_len == 1)
				{
					//stat->to_receive_len = dat;
					stat->to_receive_len = 0x3a; // fixed receive len
					stat->receive_len = 0;
					stat->receive_stat = RECEIVE_BODY;
					LOGSP("len %02x\n", dat);
				}
				break;
			}
			case RECEIVE_BODY:
				stat->buff[stat->receive_len++] = dat;
				if (stat->receive_len == stat->to_receive_len)
				{
					stat->receive_stat = RECEIVE_CRC;
					stat->receive_crc = 0xffff;
				}
				break;
			case RECEIVE_CRC:
			{
				if (stat->receive_crc == 0xffff)
				{
					stat->receive_crc = dat;
				}
				else
				{
					stat->receive_crc |= dat<<8;
					uint16_t crc = Rx_CRC_CCITT(stat->buff, stat->receive_len);
					if (crc == stat->receive_crc)
					{
						for (uint8_t i = 0; i < stat->receive_len; i++)
						{
							device.buff[i] = stat->buff[i];
						}
						stat->is_receive_newdata = 1;
						LOGSPHEX(device.buff, stat->receive_len);
						LOGSP("CRC %04x\n", stat->receive_crc);
					}
					else
					{
						LOGSP("WRONG CRC %04x %04x\n", crc, stat->receive_crc);
					}
					reset_receive(stat);
				}
				break;
			}
			case RECEIVE_END:
			{
				if (dat != 0xEF)
				{
					LOGSP("TAIL ERR%d\n", dat);
				}
				reset_receive(stat);
				break;
			}
			default:
				reset_receive(stat);
				LOGSP("ERROR STAT %d\n", stat->receive_stat);
		}
	}
}

