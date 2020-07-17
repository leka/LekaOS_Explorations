#LKExp - Mbed-OS files modifications to create .hex file instead of .bin

In `tools/targets/__init__.py`, line 796:

``` python
class DISCO_F769NICode(object):
	"""DISCO_F769NI hooks"""
	@staticmethod
	def binary_hook(t_self, resources, elf, binf):
		from intelhex import IntelHex
		binh = IntelHex()
		binh.loadbin(binf, offset = 0)
		
		with open(binf.replace(".bin", ".hex"), "w") as f:
			binh.tofile(f, format='hex')
```

In `targets/targets.json`, `"DISCO_F769NI"` section, line 2677:

``` json
        "OUTPUT_EXT": "hex",
        "post_binary_hook": {
        	"function": "DISCO_F769NICode.binary_hook"
        }
```