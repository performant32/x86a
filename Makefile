.PHONY: defualt clean config
default:
clean:
	rm -rf ./build
config:
	python3 utils/scripts/generate_config.py
