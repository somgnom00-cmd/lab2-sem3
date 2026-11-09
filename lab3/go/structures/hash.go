package structures

import (
	"fmt"
	"strconv"
	"strings"
)

type cellState uint8

const (
	empty cellState = iota
	occupied
	deleted
)

type hashCell struct {
	key, value int
	state      cellState
}

type OpenHashTable struct {
	cells      []hashCell
	size       int
	deleted    int
	doubleHash bool
}

func NewLinearHashTable(capacity int) *OpenHashTable {
	return newOpenHashTable(capacity, false)
}

func NewDoubleHashTable(capacity int) *OpenHashTable {
	return newOpenHashTable(capacity, true)
}

func newOpenHashTable(capacity int, doubleHash bool) *OpenHashTable {
	if capacity < 3 {
		capacity = 3
	}
	return &OpenHashTable{cells: make([]hashCell, nextPrime(capacity)), doubleHash: doubleHash}
}

func (h *OpenHashTable) Type() string {
	if h.doubleHash {
		return "DoubleHashTable"
	}
	return "LinearHashTable"
}

func (h *OpenHashTable) Size() int { return h.size }

func (h *OpenHashTable) Clear() {
	h.cells = make([]hashCell, len(h.cells))
	h.size, h.deleted = 0, 0
}

func magnitude(key int) uint {
	if key < 0 {
		return uint(-(key + 1)) + 1
	}
	return uint(key)
}

func foldKey(key int) int {
	number := magnitude(key)
	var sum uint
	for {
		sum += number % 1000
		number /= 1000
		if number == 0 {
			return int(sum)
		}
	}
}

func nextPrime(value int) int {
	for {
		prime := value >= 2
		for divisor := 2; prime && divisor <= value/divisor; divisor++ {
			if value%divisor == 0 {
				prime = false
			}
		}
		if prime {
			return value
		}
		value++
	}
}

func (h *OpenHashTable) step(key int) int {
	if !h.doubleHash {
		return 1
	}
	number := magnitude(key)
	return 1 + int(number%uint(len(h.cells)-1))
}

func (h *OpenHashTable) locate(key int) int {
	if len(h.cells) == 0 {
		return -1
	}
	start := foldKey(key) % len(h.cells)
	step := h.step(key)
	for attempt := 0; attempt < len(h.cells); attempt++ {
		index := (start + attempt*step) % len(h.cells)
		if h.cells[index].state == empty {
			return -1
		}
		if h.cells[index].state == occupied && h.cells[index].key == key {
			return index
		}
	}
	return -1
}

func (h *OpenHashTable) Set(key, value int) {
	if len(h.cells) == 0 {
		h.cells = make([]hashCell, 17)
	}
	if index := h.locate(key); index >= 0 {
		h.cells[index].value = value
		return
	}
	if (h.size+h.deleted+1)*10 > len(h.cells)*7 {
		capacity := len(h.cells)
		if (h.size+1)*10 > capacity*7 {
			capacity = capacity*2 + 1
		}
		h.rebuild(capacity)
	}
	start, step := foldKey(key)%len(h.cells), h.step(key)
	free := -1
	for attempt := 0; attempt < len(h.cells); attempt++ {
		index := (start + attempt*step) % len(h.cells)
		if h.cells[index].state == deleted && free < 0 {
			free = index
		}
		if h.cells[index].state == empty {
			if free < 0 {
				free = index
			}
			break
		}
	}
	if h.cells[free].state == deleted {
		h.deleted--
	}
	h.cells[free] = hashCell{key: key, value: value, state: occupied}
	h.size++
}

func (h *OpenHashTable) rebuild(capacity int) {
	old := h.cells
	h.cells = make([]hashCell, nextPrime(capacity))
	h.size, h.deleted = 0, 0
	for _, cell := range old {
		if cell.state == occupied {
			h.Set(cell.key, cell.value)
		}
	}
}

func (h *OpenHashTable) Get(key int) (int, bool) {
	index := h.locate(key)
	if index < 0 {
		return 0, false
	}
	return h.cells[index].value, true
}

func (h *OpenHashTable) Delete(key int) bool {
	index := h.locate(key)
	if index < 0 {
		return false
	}
	h.cells[index].state = deleted
	h.size--
	h.deleted++
	return true
}

func (h *OpenHashTable) Values() []string {
	result := make([]string, 0, h.size)
	for _, cell := range h.cells {
		if cell.state == occupied {
			result = append(result, fmt.Sprintf("%d=%d", cell.key, cell.value))
		}
	}
	return result
}

func (h *OpenHashTable) appendSerialized(value string) error {
	parts := strings.SplitN(value, "=", 2)
	if len(parts) != 2 {
		return fmt.Errorf("invalid hash table record")
	}
	key, err := strconv.Atoi(parts[0])
	if err != nil {
		return err
	}
	stored, err := strconv.Atoi(parts[1])
	if err != nil {
		return err
	}
	h.Set(key, stored)
	return nil
}
