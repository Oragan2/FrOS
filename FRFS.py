import sys

oFile = sys.argv[1]
iFile = sys.argv[2:]

with open(oFile, "wb") as out:
    out.write(b"FRFS")        # magic
    out.write((1).to_bytes(2, 'little'))  # version
    out.write((len(iFile)).to_bytes(2, 'little'))
    # reserve space for file table
    table_offset = out.tell()
    out.write(b'\x00' * len(iFile) * 49)
    out.write(b'\x00' * (508-((len(iFile)*49)+8%512)))
    out.write(b'SFRF')
    data_start = out.tell()
    data_offsets = []
    for f in iFile:
        with open(f, "rb") as fi:
            data = fi.read()
        offset = (out.tell() - data_start) // 512  # in sectors
        for i in range(len(data)//512+1):
            out.write(data[508*i:508*(i+1)])
            if i+1 != len(data)//512+1:
                out.write((offset+i+1).to_bytes(4,'little'))
        data_offsets.append((f, offset, len(data)//512+1))
        pad = (-len(data)) % 512
        if pad:
            out.write(b"\x00" * pad)
    # go back and write file table
    out.write(b'\x00'*2048)
    out.seek(table_offset)
    for (name, offset, size) in data_offsets:
        name = name.encode("ascii")[:31] + b'\0'
        out.write(name.ljust(32, b'\0'))
        out.write(offset.to_bytes(4, 'little'))
        out.write(size.to_bytes(4, 'little'))
        out.write(b'\x00')
        out.write(b'\x00' * 8)  # reserved
