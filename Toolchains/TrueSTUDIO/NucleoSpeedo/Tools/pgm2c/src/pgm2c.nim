
import
  os,
  re,
  strutils,
  strformat,
  tables,
  parseopt

const pix_in_row = 16

var
  pgm_file : string
  line : string

  im_width : uint
  im_height : uint
  im_colors : uint
  im_data = "uint16_t image_data[] = {\n"

  color_mark : bool
  data_sem : bool
  cnt = 0

for kind, key, val in getopt():
  case kind
  of cmdArgument:
    pgm_file = key
  of cmdLongOption, cmdShortOption: assert(false)
  of cmdEnd: assert(false)

var fd_in = pgm_file.open()

while fd_in.readLine(line):
  if line[0] == '#' or line == "P2":
    continue

  elif line =~ re"^(\d+)\s+(\d+)$":
    (im_width, im_height) = (matches[0].parseUInt, matches[1].parseUInt)
    color_mark = true

  elif color_mark:
    im_colors = line.parseUInt
    color_mark = false

  else:
    var not_1st_ln = ", " & line
    if cnt == pix_in_row:
      not_1st_ln = ",\n" & line
      cnt = 0

    cnt.inc

    im_data &= (if data_sem: not_1st_ln else: line)

    data_sem = true

fd_in.close()

im_data = &"uint16_t image_width = {im_width};\n" &
  &"uint16_t image_height = {im_height};\n" &
  &"uint16_t image_colors = {im_colors};\n" & im_data

im_data &= "\n};\n"

echo im_data

