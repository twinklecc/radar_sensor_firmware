# Makefile for generate sensor_config related code

CONFIG_FILE ?= config.csv

all: def cmd init

# definition always uses config.csv
def : sensor_code_gen.py config.csv
	@./sensor_code_gen.py -t def -o sensor_config_def.hxx config.csv

cmd : sensor_code_gen.py config.csv $(CONFIG_FILE)
	@./sensor_code_gen.py -t cmd -o sensor_config_cmd.hxx $(CONFIG_FILE)

init : sensor_code_gen.py config.csv varray1.fea varray2.fea
	@./sensor_code_gen.py --feature-list varray1 -t init -o sensor_config_init0.hxx $(CONFIG_FILE) --final-cfg varray1.tmp.csv
	@./sensor_code_gen.py --feature-list varray2 -t init -o sensor_config_init_varray2.hxx $(CONFIG_FILE) --final-cfg varray2.tmp.csv
	@./sensor_code_gen.py --feature-list varray2_1 -t init -o sensor_config_init_varray2_1.hxx $(CONFIG_FILE) --final-cfg varray2_1.tmp.csv
	@./sensor_code_gen.py --feature-list varray3 -t init -o sensor_config_init_varray3.hxx $(CONFIG_FILE) --final-cfg varray3.tmp.csv
	@./sensor_code_gen.py --feature-list varray4 -t init -o sensor_config_init_varray4.hxx $(CONFIG_FILE) --final-cfg varray4.tmp.csv
	@./sensor_code_gen.py --feature-list varray5 -t init -o sensor_config_init1.hxx $(CONFIG_FILE) --final-cfg varray5.tmp.csv
	@./sensor_code_gen.py --feature-list init2 -t init -o sensor_config_init2.hxx $(CONFIG_FILE) --final-cfg init2.tmp.csv
	@./sensor_code_gen.py --feature-list init3 -t init -o sensor_config_init3.hxx $(CONFIG_FILE) --final-cfg init3.tmp.csv
	@./sensor_code_gen.py --feature-list cas_varray2 -t init -o sensor_config_init1_cas.hxx $(CONFIG_FILE) --final-cfg init1_cas.tmp.csv
	@./sensor_code_gen.py --feature-list cas_varray1 -t init -o sensor_config_init0_cas.hxx $(CONFIG_FILE) --final-cfg init0_cas.tmp.csv

clean:
	@rm -f sensor_config_init*.hxx
	@rm -f sensor_config_cmd.hxx
	@rm -f sensor_config_def.hxx
