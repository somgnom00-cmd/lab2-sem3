package structures

import (
	"encoding/gob"
	"encoding/json"
	"fmt"
	"os"
)

type storedContainer struct {
	Type   string   `json:"type"`
	Values []string `json:"values"`
}

func SaveText(container Container, file string) (err error) {
	output, err := os.Create(file)
	if err != nil {
		return err
	}
	defer func() {
		if closeErr := output.Close(); err == nil {
			err = closeErr
		}
	}()
	encoder := json.NewEncoder(output)
	encoder.SetIndent("", "  ")
	return encoder.Encode(storedContainer{Type: container.Type(), Values: container.Values()})
}

func LoadText(container Container, file string) error {
	input, err := os.Open(file)
	if err != nil {
		return err
	}
	defer func() { _ = input.Close() }()
	var stored storedContainer
	if err := json.NewDecoder(input).Decode(&stored); err != nil {
		return err
	}
	return restore(container, stored)
}

func SaveBinary(container Container, file string) (err error) {
	output, err := os.Create(file)
	if err != nil {
		return err
	}
	defer func() {
		if closeErr := output.Close(); err == nil {
			err = closeErr
		}
	}()
	return gob.NewEncoder(output).Encode(storedContainer{Type: container.Type(), Values: container.Values()})
}

func LoadBinary(container Container, file string) error {
	input, err := os.Open(file)
	if err != nil {
		return err
	}
	defer func() { _ = input.Close() }()
	var stored storedContainer
	if err := gob.NewDecoder(input).Decode(&stored); err != nil {
		return err
	}
	return restore(container, stored)
}

func restore(container Container, stored storedContainer) error {
	if stored.Type != container.Type() {
		return fmt.Errorf("container type mismatch: %s and %s", stored.Type, container.Type())
	}
	// Заполняем временный контейнер. При ошибке прежние данные не меняются.
	switch destination := container.(type) {
	case *Array:
		next := NewArray()
		return replace(stored, next, func() { *destination = *next })
	case *SinglyList:
		next := NewSinglyList()
		return replace(stored, next, func() { *destination = *next })
	case *DoublyList:
		next := NewDoublyList()
		return replace(stored, next, func() { *destination = *next })
	case *Stack:
		next := NewStack()
		return replace(stored, next, func() { *destination = *next })
	case *Queue:
		next := NewQueue()
		return replace(stored, next, func() { *destination = *next })
	case *CompleteBinaryTree:
		next := NewCompleteBinaryTree()
		return replace(stored, next, func() { *destination = *next })
	case *OpenHashTable:
		next := newOpenHashTable(len(destination.cells), destination.doubleHash)
		return replace(stored, next, func() { *destination = *next })
	default:
		return fmt.Errorf("container does not support deserialization")
	}
}

func replace(stored storedContainer, target serializable, commit func()) error {
	for _, value := range stored.Values {
		if err := target.appendSerialized(value); err != nil {
			return err
		}
	}
	commit()
	return nil
}
