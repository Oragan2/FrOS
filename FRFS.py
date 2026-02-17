import sys

oFile = sys.argv[1]
iFile = sys.argv[2:]

with open(oFile, "wb") as out:
    out.write(b"FRFS")        # magic
    out.write((1).to_bytes(2, 'little'))  # version
    out.write((len(iFile)).to_bytes(2, 'little'))
    # reserve space for file table
    table_offset = out.tell()
    out.write(b'\x00' * len(iFile) * 104)
    out.write(b'\x00' * (512-((len(iFile)*104)%512)))
    data_start = out.tell()
    data_offsets = []
    for f in iFile:
        with open(f, "rb") as fi:
            data = fi.read()
        offset = (out.tell() - data_start) // 512  # in sectors
        out.write(data)
        data_offsets.append((f, offset, len(data)//512+1))
        pad = (-len(data)) % 512
        if pad:
            out.write(b"\x00" * pad)
    # go back and write file table
    out.seek(table_offset)
    for (name, offset, size) in data_offsets:
        name = name.encode("ascii")[:31] + b'\0'
        out.write(name.ljust(32, b'\0'))
        out.write(offset.to_bytes(4, 'little'))
        out.write(size.to_bytes(4, 'little'))
        out.write(b'\x00' * 64)  # reserved

            
