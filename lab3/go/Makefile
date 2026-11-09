.PHONY: test demo benchmark coverage lint

test:
	go test -mod=mod ./...

demo:
	go run ./cmd/lab3

benchmark:
	go test -mod=mod -bench=. -benchmem ./structures

coverage:
	go test -mod=mod -coverprofile=coverage.out ./...
	go tool cover -func=coverage.out
	go tool cover -html=coverage.out -o coverage.html
	python3 ../scripts/check_coverage.py go coverage.out

lint:
	test -z "$$(gofmt -l .)"
	go vet -mod=mod ./...
	golangci-lint run
