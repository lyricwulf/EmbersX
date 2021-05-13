package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"strings"
)

func main() {
	frag := EncodeDefine("FRAG_BINARY", "build/MidiDev/shaders/frag.spv")
	vert := EncodeDefine("VERT_BINARY", "build/MidiDev/shaders/vert.spv")

	f, err := os.Create("LyricMidiPlayer/ShaderBinaries.h")
	check(err)

	defer f.Close()

	_, err = f.WriteString(frag + "\n" + vert)
	check(err)
}

func EncodeDefine(NAME, path string) string {

	bytes, err := ioutil.ReadFile(path)
	check(err)

	var byteString string
	for _, b := range bytes {
		if b > 127 {
			byteString += "(char)" // signed unsigned goodness
		}
		byteString += fmt.Sprintf("0x%02x,", b)
	}
	byteString = TrimSuffix(byteString, ",")
	byteString = "#define " + NAME + " {" + byteString + "}"

	fmt.Println(byteString)
	return byteString
}

func check(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func TrimSuffix(s, suffix string) string {
	if strings.HasSuffix(s, suffix) {
		s = s[:len(s)-len(suffix)]
	}
	return s
}
